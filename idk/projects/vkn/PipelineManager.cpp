#include "pch.h"
#include "PipelineManager.h"
#include <res/ResourceHandle.inl>
#include <memory/ArenaAllocator.inl>
namespace idk::vkn
{
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
//// 
	VulkanPipeline& PipelineManager::GetPipeline(const pipeline_config& config, const vector<RscHandle<ShaderProgram>>& modules, uint32_t frame_index, std::optional<RenderPassObj> render_pass, bool has_depth_stencil, VulkanPipeline::Options opt)
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
		auto rp_internal = (*rp).operator VkRenderPass();
		string_view str{ r_cast<const char*>(&rp_internal),sizeof(rp_internal) };
		combi += str; //Add renderpass as a part of the unique id.
		uvec2 viewport_size{};
		ivec2 viewport_offset{};
		if (config.viewport_size)
			viewport_size = *config.viewport_size;
		if (config.viewport_offset)
			viewport_offset = *config.viewport_offset;
		combi += string_view{ r_cast<const char*>(&viewport_offset), sizeof(viewport_offset) };
		combi += string_view{ r_cast<const char*>(&viewport_size), sizeof(viewport_size)};
		for (auto& desc : config.buffer_descriptions)
			combi += desc.GenString();

		decltype(opt.derive_from->base) derived = {};
		derived = (opt.derive_from) ? opt.derive_from->base:derived;
		char a = opt.is_base_pipeline?1:0;

		combi += a;
		combi += string_view{ r_cast<const char*>(&derived), sizeof(derived) };

		for (auto dyn_state : opt.dynamic_states)
			combi += string_view{ r_cast<const char*>(&dyn_state), sizeof(dyn_state) };

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
			vector<vk::ShaderModule> cached_modules;
			cached_modules.reserve(modules.size());
			for (auto& module : modules)
			{
				cached_modules.emplace_back(module.as<ShaderModule>().Module());
			}

			PipelineObject obj{ config,render_pass,has_depth_stencil,modules,cached_modules };
			obj.StoreBufferDescOverrides();
			obj.Create(View(),frame_index,opt);

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
		for (auto po_itr = pipelines.begin(),end = pipelines.end();po_itr!=end;++po_itr )
		{
			auto& po = *po_itr;
			auto obj_handle = po_itr.handle();
			if (&po.pipeline == pipeline)
			{
				for (auto& shader : po.shader_handles)
				{
					prog_to_pipe.erase(shader);
				}
				vector<const string*> keys;
				for (auto& [str, handle] : prog_to_pipe2)
				{
					if (handle == obj_handle)
					{
						keys.emplace_back(&str);
					}
				}
				for (auto& key : keys)
				{
					prog_to_pipe2.erase(*key);
				}
				pipelines.free(obj_handle);
				for (auto& queue : update_queue)
				{
					for (auto itr = queue.begin(); queue.end() != itr; ++itr)
					{
						if (*itr == obj_handle)
						{
							queue.erase(itr);
							break;
						}
					}
				}
				break;
			}
		}

	}

	bool HasInvalidHandles(const vector<RscHandle<ShaderProgram>>& shaders, const vector<vk::ShaderModule>& shaders_concrete)
	{
		bool invalid = false;
		for (size_t i=0;i< shaders.size();++i)
		{
			auto& shader = shaders[i];
			auto concrete = shaders_concrete[i];
			invalid |= !shader || !shader.as<ShaderModule>().HasCurrent()|| shader.as<ShaderModule>().Module()!=concrete;
		}
		return invalid;
	}

//// 
	void PipelineManager::CheckForUpdates(uint32_t frame_index)
	{
		vector<size_t> pipelines_to_update;//handles
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
			if (itr->rp && (!*itr->rp || HasInvalidHandles(itr->shader_handles,itr->shader_concrete)))
			{
				pipelines_to_destroy.emplace(&itr->pipeline);
			}
		}
		/*
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
				pipelines_to_update.emplace_back(itr.handle());
			}
		}
		*/
		for (auto& itr : pipelines_to_destroy)
		{
			RemovePipeline(itr);
		}/*
		for (auto handle : pipelines_to_update)
		{
			pipelines.free(handle);
		}*/
		if(pipelines.capacity() < 100 + (pipelines.size()*3) / 2)
			pipelines.reserve(100 + pipelines.size() * 2);
	}

	//PipelineObject() = default;
	//PipelineObject(PipelineObject&&) noexcept= default;
	//~PipelineObject() = default;

	void PipelineManager::PipelineObject::StoreBufferDescOverrides()
	{
		desc_helper.StoreBufferDescOverrides(config);
	}

	void PipelineManager::PipelineObject::Create(VulkanView& view,[[maybe_unused]] size_t fo_index, VulkanPipeline::Options opt)
	{
		//TODO: set the pipeline's modules
		desc_helper.UseShaderAttribs(shader_handles, config);
		if (rp && *rp)
			pipeline.SetRenderPass(**rp, has_depth_stencil);
		else
			pipeline.ClearRenderPass();
		pipeline.Create(config, shader_handles, view,opt);
	}
	void PipelineManager::PipelineObject::Swap()
	{
		std::swap(pipeline, back_pipeline);
	}
}