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
					auto& mod = module.as<ShaderModule>();
					if (mod.NeedUpdate())
						mod.Update();
					auto& desc = mod.AttribDescriptions();
					for(auto& desc_set : desc)
						config.buffer_descriptions.emplace_back(desc_set);
					shaders.emplace_back(mod.Stage(), mod.Module());
					mod.ApplyUniformToConfig(config);
				}
				pipeline.Create(config, shaders, view);
				for (auto& module : shader_handles)
				{
					for (auto& layout : pipeline.uniform_layouts)
					{
						auto& mod = module.as<ShaderModule>();
						mod.SetLayout(layout.first, *layout.second);
					}
				}
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