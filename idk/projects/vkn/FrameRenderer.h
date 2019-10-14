#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VulkanView.h>
#include <vkn/UboManager.h>
#include <vkn/VulkanPipeline.h>
#include <gfx/pipeline_config.h>
#include <vkn/VknFrameBufferManager.h>

#include <vkn/RenderStateV2.h>
#include <vkn/ProcessedRO.h>

#include <vkn/PipelineThingy.h>

namespace idk
{
	struct RenderObject;
}
namespace idk::vkn
{

	struct GraphicsState;
	class PipelineManager;
	using PipelineHandle_t =uint32_t;
	class FrameRenderer
	{
	public:
		
		void Init(VulkanView* view, vk::CommandPool cmd_pool);

		void SetPipelineManager(PipelineManager& manager);
		void RenderGraphicsStates(const vector<GraphicsState>& state,uint32_t frame_index);
		PresentationSignals& GetMainSignal();
	private:
		struct VertexUniformConfig;
		using ProcessedRO=vkn::ProcessedRO;
		using DsBindingCount =hash_table<vk::DescriptorSetLayout, std::pair<vk::DescriptorType, uint32_t>>;
		class IRenderThread
		{
		public:
			IRenderThread() = default;
			IRenderThread(const IRenderThread&) = delete;
			IRenderThread(IRenderThread&&) = default;
			IRenderThread&operator=(const IRenderThread&) = delete;
			IRenderThread&operator=(IRenderThread&&) = default;
			virtual void Render(const GraphicsState& state, RenderStateV2& rs) = 0;
			virtual void Join() = 0;
			virtual ~IRenderThread()=default;
		};
		class NonThreadedRender : public IRenderThread
		{
		public:
			void Init(FrameRenderer* renderer);
			void Render(const GraphicsState& state, RenderStateV2& rs)override;
			void Join() override;
		private:
			FrameRenderer* _renderer;
		};

		void GrowStates(size_t new_min_size);

		PipelineThingy ProcessRoUniforms(const GraphicsState& draw_calls, UboManager& ubo_manager);
		void RenderGraphicsState(const GraphicsState& state, RenderStateV2& rs);
		void RenderDebugStuff(const GraphicsState& state,RenderStateV2& rs);
		VulkanView& View()const { return *_view; }
		vk::RenderPass GetRenderPass(const GraphicsState& state, VulkanView& view);
		
		RscHandle<ShaderProgram> GetMeshRendererShaderModule();
		PipelineManager& GetPipelineManager();
		VulkanPipeline&  GetPipeline(const pipeline_config& config,const vector<RscHandle<ShaderProgram>>& prog);
		//PipelineHandle_t GetPipelineHandle();//Add arguments when we know what determines which pipeline to get.

		uint32_t _current_frame_index;

		VulkanView*                            _view                       {};
		RscHandle<ShaderProgram>               _shadow_shader_module{};
		PipelineManager*                       _pipeline_manager           {};
		vector<RenderStateV2>                  _states                     {};
		const vector<GraphicsState>*           _gfx_states                 {};
		vector<vk::UniqueCommandBuffer>        _state_cmd_buffers          {};
		vector<std::unique_ptr<IRenderThread>> _render_threads             {};
		vk::CommandPool         _cmd_pool{};
		vk::UniqueCommandBuffer _pri_buffer{};
		vk::UniqueCommandBuffer _transition_buffer{};
		//VknFrameBufferManager   fb_man{};
	};
}
