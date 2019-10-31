#pragma once

#include <vkn/VknFrameBuffer.h>
#include <vkn/PipelineThingy.h>
#include <gfx/RenderObject.h>
namespace idk::vkn
{
	struct GraphicsState;
	struct RenderStateV2;
	class PipelineManager;

	enum class GBufferBinding
	{
		eAlbedoAmbOcc       ,
		eUvMetallicRoughness,
		eViewPos			,
		eNormal				,
		eTangent			,
	};
	struct DeferredGBuffer
	{
		RscHandle<VknFrameBuffer> gbuffer;
		vk::Semaphore RenderCompleteSignal();
		void Init(ivec2 size);
	private:
		vk::UniqueSemaphore _render_complete;
	};

	struct DeferredPass
	{
		DeferredGBuffer _gbuffer;
		PipelineManager* _pipeline_manager;
		RenderObject fsq_ro;
		RscHandle<ShaderProgram> fullscreen_quad_vert;
		RscHandle<ShaderProgram> deferred_post_frag;

		uint32_t _frame_index = 0;
		uint32_t frame_index(uint32_t new_index){ return _frame_index = new_index; }
		uint32_t frame_index()const { return _frame_index; }
		PipelineManager& pipeline_manager(PipelineManager& manager) { return *(_pipeline_manager = &manager); }
		PipelineManager& pipeline_manager()const
		{
			return *_pipeline_manager;
		}
		DeferredGBuffer& GBuffer() {
			return _gbuffer;
		};
		//Make sure to call this again if the framebuffer size changed.
		void Init(ivec2 size);
		//void RenderGbufferToTarget(vk::CommandBuffer cmd_buffer, const GraphicsState& graphics_state, RenderStateV2& rs);
		void BindGBuffers(const GraphicsState& graphics_state, RenderStateV2& rs);
		PipelineThingy ProcessDrawCalls(const GraphicsState& graphics_state, RenderStateV2& rs);
		void DrawToGBuffers(vk::CommandBuffer cmd_buffer, const GraphicsState& graphics_state, RenderStateV2& rs);
	};

}