#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/object_pool.h>
#include <gfx/pipeline_config.h>
#include <vkn/ShaderModule.h>
#include <vkn/VulkanPipeline.h>
namespace idk::vkn
{
	class PipelineManager
	{
		struct PipelineObject;
		using container_t = object_pool<PipelineObject>;
	public:
		using handle_t = container_t::handle_t;
		void View(VulkanView& view);
		VulkanView& View();
		//Assumes that shader programs are the only differing thing.
		VulkanPipeline& GetPipeline(const pipeline_config& config, const vector<RscHandle<ShaderProgram>>& modules);
		void CheckForUpdates(uint32_t frame_index);
	private:
		struct PipelineObject
		{
			pipeline_config config;
			vector<RscHandle<ShaderProgram>> shader_handles;
			VulkanPipeline pipeline;
			VulkanPipeline back_pipeline;

			//PipelineObject() = default;
			//PipelineObject(PipelineObject&&) noexcept= default;
			//~PipelineObject() = default;
			void Create(VulkanView& view)
			{
				//TODO: set the pipeline's modules
				vector<std::pair<vk::ShaderStageFlagBits, vk::ShaderModule>> shaders;
				for (auto& module : shader_handles)
				{
					shaders.emplace_back(module.as<ShaderModule>().Stage(), module.as<ShaderModule>().Module());
				}
				pipeline.Create(config, shaders, view);
			}
			void Swap()
			{
				std::swap(pipeline, back_pipeline);
			}
		};
		container_t pipelines;
		hash_table<RscHandle<ShaderProgram>, handle_t> prog_to_pipe;
		vector<vector<handle_t>> update_queue;
		VulkanView* _view;
	};
}