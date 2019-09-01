#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VulkanView.h>
#include <vkn/UboManager.h>
#include <vkn/VulkanPipeline.h>
namespace idk
{
	struct GraphicsState;
}
namespace idk::vkn
{
	struct RenderStateV2
	{
		vk::CommandBuffer cmd_buffer;
		UboManager ubo_manager;//Should belong to each thread group.
		bool has_commands = false;
		void FlagRendered() { has_commands = true; }
		void Reset();
		RenderStateV2() = default;
		RenderStateV2(const RenderStateV2&) = delete;
		RenderStateV2(RenderStateV2&&) = default;
	};
	using PipelineHandle_t =uint32_t;
	struct RenderFrameObject
	{
	};
	class FrameRenderer
	{
	public:
		
		void Init(VulkanView* view, vk::CommandPool cmd_pool);


		void RenderGraphicsStates(const vector<GraphicsState>& state);
	private:
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

		void RenderGraphicsState(const GraphicsState& state,RenderStateV2& rs);
		VulkanView& View()const { return *_view; }
		VulkanView* _view = nullptr;
		VulkanPipeline&  GetPipeline(PipelineHandle_t);
		PipelineHandle_t GetPipelineHandle();//Add arguments when we know what determines which pipeline to get.

		vector<RenderStateV2> _states{};
		const vector<GraphicsState>* _gfx_states{};
		vector<vk::UniqueCommandBuffer> _state_cmd_buffers;
		vector<std::unique_ptr<IRenderThread>> _render_threads;
		vk::CommandPool         _cmd_pool;
		vk::UniqueCommandBuffer _pri_buffer{};
		vk::UniqueCommandBuffer _transition_buffer{};
	};
}
