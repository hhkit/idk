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

#include <vkn/RenderUtil.h>
#include <vkn/PipelineThingy.h>

#include <vkn/GraphicsState.h>

#include <vkn/CubemapRenderer.h>
#include <vkn/ParticleRenderer.h>
#include <vkn/FontRenderer.h>
#include <vkn/CanvasRenderer.h>

namespace idk
{
	struct RenderObject;
	struct LightData;
}
namespace idk::vkn
{
	struct PreRenderData;
	struct GraphicsState;
	class PipelineManager;
	using PipelineHandle_t =uint32_t;
	class FrameRenderer
	{
	public:
		
		void Init(VulkanView* view, vk::CommandPool cmd_pool);

		void SetPipelineManager(PipelineManager& manager);
		void PreRenderGraphicsStates(const PreRenderData& state, uint32_t frame_index);
		void RenderGraphicsStates(const vector<GraphicsState>& state,uint32_t frame_index);
		void PostRenderGraphicsStates(const PostRenderData& state, uint32_t frame_index);
		vk::UniqueSemaphore& GetPostRenderComplete() { return _post_render_complete; }
		PresentationSignals& GetMainSignal();
		SharedGraphicsState shared_graphics_state;
		void ColorPick(vector<ColorPickRequest>&& pick_buffer);
		void RenderGraphicsState(const GraphicsState& state, RenderStateV2& rs);
		
		FrameRenderer() = default;
		FrameRenderer(const FrameRenderer&)= delete;
		FrameRenderer(FrameRenderer&&);
		~FrameRenderer();
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
		class ThreadedRender;

		void GrowStates(vector<RenderStateV2>& states, size_t new_min_size);

		PipelineThingy ProcessRoUniforms(const GraphicsState& draw_calls, UboManager& ubo_manager);
		void RenderDebugStuff(const GraphicsState& state,RenderStateV2& rs ,ivec2 vp_pos, ivec2 vp_size);

		//PreRender
		void PreRenderShadow(size_t light_index, const PreRenderData& state, vector<RenderStateV2>& rs,size_t& curr_state, uint32_t frame_index);
		
		//PostRender
		void PostRenderCanvas(size_t& canvas_count, RscHandle<RenderTarget> rt, const vector<UIRenderObject>& canvas_data, const PostRenderData& state, RenderStateV2& rs, uint32_t frame_index);


		VulkanView& View()const { return *_view; }
		vk::RenderPass GetRenderPass(const GraphicsState& state, VulkanView& view);
		
		RscHandle<ShaderProgram> GetMeshRendererShaderModule();
		PipelineManager& GetPipelineManager();
		VulkanPipeline&  GetPipeline(const pipeline_config& config,const vector<RscHandle<ShaderProgram>>& prog);
		//PipelineHandle_t GetPipelineHandle();//Add arguments when we know what determines which pipeline to get.

		uint32_t _current_frame_index;

		VulkanView*                            _view                       {};
		RscHandle<ShaderProgram>               _shadow_shader_module       {};
		PipelineManager*                       _pipeline_manager           {};
		vector<RenderStateV2>                  _states                     {};
		vector<RenderStateV2>                  _pre_states                 {};
		vector<RenderStateV2>                  _post_states                {};
		const vector<GraphicsState>*           _gfx_states                 {};		
		vector<vk::UniqueCommandBuffer>        _state_cmd_buffers          {};
		vector<std::unique_ptr<IRenderThread>> _render_threads             {};
		vk::UniqueSemaphore                    _pre_render_complete        {};
		vk::UniqueSemaphore                    _post_render_complete       {};
		vk::CommandPool         _cmd_pool{};
		vk::UniqueCommandBuffer _pri_buffer{};
		vk::UniqueCommandBuffer _transition_buffer{};
		
		
		
		////////Certain obj renderers///////
		CubemapRenderer _convoluter;
		CubemapRenderer _skybox;	
		ParticleRenderer _particle_renderer;
		FontRenderer   _font_renderer;
		CanvasRenderer _canvas_renderer;

		DeferredGBuffer _gbuffers[EGBufferType::size()];
		ivec2 _gbuffer_size{};

		struct PImpl;
		shared_ptr<PImpl> _pimpl;

		//VknFrameBufferManager   fb_man{};
	};
}
