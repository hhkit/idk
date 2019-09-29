#include "pch.h"
#include "PipelineManager.h"
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
	VulkanPipeline& PipelineManager::GetPipeline(const pipeline_config& config, const vector<RscHandle<ShaderProgram>>& modules,uint32_t frame_index)
	{
		std::optional<handle_t> prev{};
		bool is_diff = prog_to_pipe2.empty();
		string combi;
		size_t arr[16] = {};
		for (size_t i = 0; i < modules.size(); ++i)
		{
			arr[i] = i;
			for (size_t j = 0; j < i; ++j)
				if (Less(modules[arr[i]] , modules[arr[j]]))
					std::swap(arr[i], arr[j]);
		}
		for (size_t i = 0; i < modules.size(); ++i)
		{
			combi += modules[arr[i]].guid.operator idk::string();
		}
		auto itr = prog_to_pipe2.find(combi);
		if (itr != prog_to_pipe2.end())
		{
			prev = itr->second;
		}
		else
		{
			is_diff = true;
		}
		
		if (is_diff)
		{
			PipelineObject obj{ config,modules };
			obj.Create(View(),frame_index);

			//TODO threadsafe lock here
			auto handle = pipelines.add(std::move(obj));
			prog_to_pipe2.emplace(combi,handle);
			prev = handle;
		}
		return pipelines.get(*prev).pipeline;
	}
	void PipelineManager::CheckForUpdates(uint32_t frame_index)
	{
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

		for (auto itr = pipelines.begin(); itr != pipelines.end(); ++itr)
		{
			auto handle = itr.handle();
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
			}
			if (need_update)
			{
				//Recreate pipeline in back_pipeline
				pipelines[handle].Swap();
				pipelines[handle].Create(View(),frame_index);
			}
		}
	}
}