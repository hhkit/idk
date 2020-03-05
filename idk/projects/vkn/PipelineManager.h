#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/object_pool.h>
#include <gfx/pipeline_config.h>
#include <vkn/ShaderModule.h>
#include <vkn/VulkanPipeline.h>
#include <parallel/multithread_control.h>
#include <vkn/PipelineDescHelper.h>
namespace idk::vkn
{
	class PipelineManager
	{
		struct PipelineObject;
		using container_t = object_pool<PipelineObject>;
	public:
		using handle_t = container_t::handle_t;
		//Assumes that shader programs are the only differing thing.
		VulkanPipeline& GetPipeline(const pipeline_config& config, const vector<RscHandle<ShaderProgram>>& modules, uint32_t frame_index, std::optional<RenderPassObj> render_pass = {}, bool has_depth_stencil = false, VulkanPipeline::Options opt = {});
		void CheckForUpdates(uint32_t frame_index);
		void RemovePipeline(VulkanPipeline* pipeline);
	private:
		struct PipelineObject
		{
			pipeline_config config{};
			std::optional<RenderPassObj> rp{};
			bool has_depth_stencil = false;
			vector<RscHandle<ShaderProgram>> shader_handles;
			vector<vk::ShaderModule> shader_concrete;
			VulkanPipeline pipeline;
			VulkanPipeline back_pipeline;

			PipelineDescHelper desc_helper;


			void StoreBufferDescOverrides();


			//PipelineObject() = default;
			//PipelineObject(PipelineObject&&) noexcept= default;
			//~PipelineObject() = default;
			void Create(VulkanView& view, [[maybe_unused]] size_t fo_index, VulkanPipeline::Options opt);
			void Swap();
		};
		//std::atomic_bool creating = false;
		container_t pipelines;
		raynal_rw_lock _lock;
		hash_table<string, handle_t> prog_to_pipe2;
		hash_table<RscHandle<ShaderProgram>, handle_t> prog_to_pipe;
		vector<vector<handle_t>> update_queue;
		VulkanView* _view;
	};
}