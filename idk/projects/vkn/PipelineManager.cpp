#include "pch.h"
#include "PipelineManager.h"
#include <res/ResourceHandle.inl>
#include <memory/ArenaAllocator.inl>
namespace idk::vkn
{
	void PipelineManager::View(VulkanView& view)
	{
		_view = &view;
	}
	VulkanView& PipelineManager::View()
	{
		return *_view;
	}
	template<typename P>
	bool LessChain(P clhs, P crhs)
	{
		return clhs < crhs;
	}
	template<typename P,  typename ...Args>
	bool LessChain(P clhs, P crhs, Args... args)
	{
		return clhs < crhs || (clhs == crhs && LessChain(args...));
	}
	
	bool Less(const RscHandle<ShaderProgram>& lhs, const RscHandle<ShaderProgram>& rhs)
	{
		return LessChain(lhs.guid.Data1, rhs.guid.Data1, lhs.guid.Data2, rhs.guid.Data2, lhs.guid.Data3, rhs.guid.Data3, lhs.guid.Data4, rhs.guid.Data4);
			   //LessChain(lhs.guid.Data1,rhs.guid.Data1,lhs.guid.Data2 , rhs.guid.Data2)|| && lhs.guid.Data3 < rhs.guid.Data3 && reinterpret_cast<uint64_t>(lhs.guid.Data4) < reinterpret_cast<uint64_t>(rhs.guid.Data4);
	}
//#pragma optimize("",off)
	VulkanPipeline& PipelineManager::GetPipeline(const pipeline_config& config, const vector<RscHandle<ShaderProgram>>& modules, uint32_t frame_index, std::optional<RenderPassObj> render_pass, bool has_depth_stencil)
	{
		std::optional<handle_t> prev{};
		bool is_diff = prog_to_pipe2.empty();

		char buffer[16384] = {};

		ArenaAllocator<char> allocator{buffer};
		std::basic_string<char, std::char_traits<char>, ArenaAllocator<char>> combi{ allocator };
		size_t arr[16] = {};
		for (size_t i = 0; i < modules.size(); ++i)
		{
			arr[i] = i;
			for (size_t j = 0; j < i; ++j)
				if (Less(modules[arr[i]], modules[arr[j]]))
					std::swap(arr[i], arr[j]);
		}
		for (size_t i = 0; i < modules.size(); ++i)
		{
			auto module = modules[arr[i]].as<ShaderModule>().Module().operator VkShaderModule();
			combi.append(r_cast<const char*>(&module),sizeof(module));
		}

		RenderPassObj rp = View().BasicRenderPass(config.render_pass_type);
		if (render_pass)
			rp = *render_pass;
		string_view str{ r_cast<const char*>(&rp),sizeof(rp) };
		combi += str; //Add renderpass as a part of the unique id.
		ivec2 viewport_size{}, viewport_offset{};
		if (config.viewport_size)
			viewport_size = *config.viewport_size;
		if (config.viewport_offset)
			viewport_offset = *config.viewport_offset;
		combi += string_view{ r_cast<const char*>(&viewport_offset), sizeof(viewport_offset) };
		combi += string_view{ r_cast<const char*>(&viewport_size), sizeof(viewport_size)};
		for (auto& desc : config.buffer_descriptions)
			combi += desc.GenString();

		auto completed_combi = string{ combi.data(),combi.size() };
		_lock.begin_read();
		auto itr = prog_to_pipe2.find(completed_combi);
		if (itr != prog_to_pipe2.end())
		{
			prev = itr->second;
		}
		else
		{
			is_diff = true;
		}
		_lock.end_read();

		if (is_diff)
		{
			PipelineObject obj{ config,render_pass,has_depth_stencil,modules };
			obj.StoreBufferDescOverrides();
			obj.Create(View(),frame_index);

			//TODO threadsafe lock here
			//while (!creating.compare_exchange_strong(curr_expected_val, true));
			_lock.begin_write();
			auto handle = pipelines.add(std::move(obj));
			
			prog_to_pipe2.emplace(completed_combi,handle);
			prev = handle;
			_lock.end_write();
			//creating.store(false);
		}
		_lock.begin_read();
		auto& result = pipelines.get(*prev).pipeline;
		_lock.end_read();
		return result;
	}
	void PipelineManager::RemovePipeline(VulkanPipeline* pipeline)
	{
		size_t index = 0;
		for (auto& po : pipelines)
		{
			if (&po.pipeline == pipeline)
			{
				for (auto& shader : po.shader_handles)
				{
					prog_to_pipe.erase(shader);
				}
				vector<const string*> keys;
				for (auto& [str, handle] : prog_to_pipe2)
				{
					if (handle == index)
					{
						keys.emplace_back(&str);
					}
				}
				for (auto& key : keys)
				{
					prog_to_pipe2.erase(*key);
				}
				pipelines.free(index);
				for (auto& queue : update_queue)
				{
					for (auto itr = queue.begin(); queue.end() != itr; ++itr)
					{
						if (*itr == index)
						{
							queue.erase(itr);
							break;
						}
					}
				}
				break;
			}
			++index;
		}

	}

	bool HasInvalidHandles(const vector<RscHandle<ShaderProgram>>& shaders)
	{
		bool invalid = false;
		for (auto& shader : shaders)
		{
			invalid |= (!shader || (!shader.as<ShaderModule>().operator bool() && !shader.as<ShaderModule>().HasUpdate()));
		}
		return invalid;
	}

//#pragma optimize("",off)
	void PipelineManager::CheckForUpdates(uint32_t frame_index)
	{
		vector<decltype(pipelines)::iterator> pipelines_to_update;
		hash_set<VulkanPipeline*> pipelines_to_destroy;
		pipelines_to_update.reserve(pipelines.size() * 2);
		pipelines_to_destroy.reserve(pipelines.size());

		if (frame_index >= update_queue.size())
		{
			update_queue.resize(s_cast<size_t>(frame_index) + 1);
		}
		auto& curr_queue = update_queue[frame_index];
		//Clear the previous ones
		for (auto handle : curr_queue)
		{
			pipelines[handle].back_pipeline.Reset(); //Destroy old pipeline
		}
		//Remove invalid pipelines.
		for (auto itr = pipelines.begin(); itr != pipelines.end(); ++itr)
		{
			if (itr->rp && (!*itr->rp || HasInvalidHandles(itr->shader_handles)))
			{
				pipelines_to_destroy.emplace(&itr->pipeline);
			}
		}

		for (auto& itr : pipelines_to_destroy)
		{
			RemovePipeline(itr);
		}
		//Update pipelines that need to be updated.
		for (auto itr = pipelines.begin(); itr != pipelines.end(); ++itr)
		{
			auto& po = *itr;
			bool need_update = false;
			for (auto& shader : po.shader_handles)
			{
				auto& module = shader.as<ShaderModule>();
				if (module.NeedUpdate())
				{
					need_update = true;
					module.UpdateCurrent(frame_index);
				}
				need_update |= module.NewlyLoaded();
			}
			if (need_update)
			{
				pipelines_to_update.emplace_back(itr);
			}
		}
		for (auto itr : pipelines_to_update)
		{
			auto& po = *itr;
			auto handle = itr.handle();
			for (auto& shader : po.shader_handles)
			{
				auto& module = shader.as<ShaderModule>();
				if (module.NeedUpdate())
				{
					module.UpdateCurrent(frame_index);
				}

				module.NewlyLoaded(false);
			}
			{
				//Recreate pipeline in back_pipeline
				pipelines[handle].Swap();
				pipelines[handle].Create(View(), frame_index);
			}
		}
		pipelines.reserve(100 + pipelines.size() * 2);
	}

	//PipelineObject() = default;
	//PipelineObject(PipelineObject&&) noexcept= default;
	//~PipelineObject() = default;

	void PipelineDescHelper::StoreBufferDescOverrides(const pipeline_config& config)
	{
		buffer_desc_overrides = config.buffer_descriptions;
		size_t i = 0;
		for (auto& desc : buffer_desc_overrides)
		{
			for (auto& attr : desc.attributes)
			{
				override_attr_mapping[attr.location] = i;
			}
			++i;
		}
	}

	void PipelineDescHelper::ApplyBufferDescOverrides(pipeline_config& config)
	{
		vector<size_t> binding_removal_indices;
		auto& config_bdesc = config.buffer_descriptions;
		size_t i = 0;
		for (auto& cbd : config_bdesc)
		{
			for (auto& attr : cbd.attributes)
			{
				auto itr = override_attr_mapping.find(attr.location);
				if (itr != override_attr_mapping.end())
				{
					binding_removal_indices.emplace_back(i);
				}
			}
			++i;
		}
		std::sort(binding_removal_indices.begin(), binding_removal_indices.end());
		{
			auto end_itr = std::unique(binding_removal_indices.begin(), binding_removal_indices.end());
			binding_removal_indices.resize(end_itr - binding_removal_indices.begin());
		}
		//erase from behind to avoid invalidating indices
		for (auto itr = binding_removal_indices.rbegin(); itr < binding_removal_indices.rend(); ++itr)
		{
			auto index = *itr;
			config_bdesc.erase(config_bdesc.begin()+index);
		}
		config_bdesc.insert(config_bdesc.end(), buffer_desc_overrides.begin(), buffer_desc_overrides.end());

	}

	void PipelineManager::PipelineObject::StoreBufferDescOverrides()
	{
		desc_helper.StoreBufferDescOverrides(config);
	}

	void PipelineManager::PipelineObject::Create(VulkanView& view,[[maybe_unused]] size_t fo_index)
	{
		//TODO: set the pipeline's modules
		desc_helper.UseShaderAttribs(shader_handles, config);
		if (rp && *rp)
			pipeline.SetRenderPass(**rp, has_depth_stencil);
		else
			pipeline.ClearRenderPass();
		pipeline.Create(config, shader_handles, view);
	}
	void PipelineManager::PipelineObject::Swap()
	{
		std::swap(pipeline, back_pipeline);
	}
}