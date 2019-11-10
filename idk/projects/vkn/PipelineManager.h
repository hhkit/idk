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
		VulkanPipeline& GetPipeline(const pipeline_config& config, const vector<RscHandle<ShaderProgram>>& modules, uint32_t frame_index, std::optional<vk::RenderPass> render_pass = {},bool has_depth_stencil=false);
		void CheckForUpdates(uint32_t frame_index);
		void RemovePipeline(VulkanPipeline* pipeline);
	private:
		struct PipelineObject
		{
			pipeline_config config{};
			std::optional<vk::RenderPass> rp{};
			bool has_depth_stencil = false;
			vector<RscHandle<ShaderProgram>> shader_handles;
			VulkanPipeline pipeline;
			VulkanPipeline back_pipeline;

			vector<buffer_desc> buffer_desc_overrides;
			hash_table<uint32_t, size_t> override_attr_mapping;

			void StoreBufferDescOverrides();

			void ApplyBufferDescOverrides();

			//PipelineObject() = default;
			//PipelineObject(PipelineObject&&) noexcept= default;
			//~PipelineObject() = default;
			void Create(VulkanView& view, [[maybe_unused]] size_t fo_index);
			void Swap();
		};
		container_t pipelines;

		hash_table<string, handle_t> prog_to_pipe2;
		hash_table<RscHandle<ShaderProgram>, handle_t> prog_to_pipe;
		vector<vector<handle_t>> update_queue;
		VulkanView* _view;
	};
}