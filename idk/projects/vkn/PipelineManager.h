#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/object_pool.h>
#include <gfx/pipeline_config.h>
#include <vkn/ShaderModule.h>
#include <vkn/VulkanPipeline.h>
#include <multithread_control.h>
namespace idk::vkn
{
	struct PipelineDescHelper
	{

		vector<buffer_desc> buffer_desc_overrides;
		hash_table<uint32_t, size_t> override_attr_mapping;

		//Store first
		void StoreBufferDescOverrides(const pipeline_config& config);
		//Does not help you store your overrides.
		void UseShaderAttribs(const vector<RscHandle<ShaderProgram>>& shader_handles, pipeline_config& config)
		{
			config.buffer_descriptions.clear();
			for (auto& module : shader_handles)
			{
				auto& mod = module.as<ShaderModule>();
				if (!mod.HasCurrent())
					continue;
				//if (mod.NeedUpdate()) //Excluded. leave it to pipeline manager's check for update.
				//	mod.UpdateCurrent(fo_index);
				auto& desc = mod.AttribDescriptions();
				for (auto& desc_set : desc)
					config.buffer_descriptions.emplace_back(desc_set);
				//shaders.emplace_back(mod.Stage(), mod.Module());
				if (mod.Stage() == vk::ShaderStageFlagBits::eFragment)
					config.frag_shader = module;

				if (mod.Stage() == vk::ShaderStageFlagBits::eVertex)
					config.vert_shader = module;
				ApplyBufferDescOverrides(config);
			}
		}
	private:
		void ApplyBufferDescOverrides(pipeline_config& config);
	};
	class PipelineManager
	{
		struct PipelineObject;
		using container_t = object_pool<PipelineObject>;
	public:
		using handle_t = container_t::handle_t;
		void View(VulkanView& view);
		VulkanView& View();
		//Assumes that shader programs are the only differing thing.
		VulkanPipeline& GetPipeline(const pipeline_config& config, const vector<RscHandle<ShaderProgram>>& modules, uint32_t frame_index, std::optional<RenderPassObj> render_pass = {},bool has_depth_stencil=false);
		void CheckForUpdates(uint32_t frame_index);
		void RemovePipeline(VulkanPipeline* pipeline);
	private:
		struct PipelineObject
		{
			pipeline_config config{};
			std::optional<RenderPassObj> rp{};
			bool has_depth_stencil = false;
			vector<RscHandle<ShaderProgram>> shader_handles;
			VulkanPipeline pipeline;
			VulkanPipeline back_pipeline;

			PipelineDescHelper desc_helper;


			void StoreBufferDescOverrides();


			//PipelineObject() = default;
			//PipelineObject(PipelineObject&&) noexcept= default;
			//~PipelineObject() = default;
			void Create(VulkanView& view, [[maybe_unused]] size_t fo_index);
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