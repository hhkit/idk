#pragma once
#include <vkn/FrameRenderer.h>

#include <vkn/VknFrameBuffer.h>
namespace idk::vkn
{
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
		DeferredGBuffer* _gbuffer;
		DeferredGBuffer& GBuffer()const {
			return *_gbuffer;
		};
		void RenderGbufferToTarget(vk::CommandBuffer cmd_buffer, const GraphicsState& graphics_state, RenderStateV2& rs);
		void BindGBuffers(const GraphicsState& graphics_state, RenderStateV2& rs);
		void DrawToGBuffers(vk::CommandBuffer cmd_buffer, const GraphicsState& graphics_state, RenderStateV2& rs);
	};

}