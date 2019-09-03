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
	VulkanPipeline& PipelineManager::GetPipeline(const pipeline_config& config, const vector<RscHandle<ShaderProgram>>& modules)
	{
		bool is_diff = prog_to_pipe.empty();
		std::optional<handle_t> prev{};
		for (auto& module : modules)
		{
			auto itr = prog_to_pipe.find(module);
			is_diff = (prev && itr != prog_to_pipe.end() && itr->second == *prev);
			prev = itr->second;
		}
		if (is_diff)
		{
			PipelineObject obj{ config,modules };
			obj.Create(View());
			auto handle = pipelines.add(std::move(obj.pipeline));
			for (auto& module : modules)
			{
				prog_to_pipe.emplace(module, handle);
			}
			prev = handle;
		}
		return pipelines.get(*prev).pipeline;
	}
	void PipelineManager::CheckForUpdates(uint32_t frame_index)
	{
		if (frame_index >= update_queue.size())
		{
			update_queue.resize(frame_index + 1);
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
					module.Update();
				}
			}
			if (need_update)
			{
				//Recreate pipeline in back_pipeline
				pipelines[handle].Swap();
				pipelines[handle].Create(View());
			}
		}
	}
}