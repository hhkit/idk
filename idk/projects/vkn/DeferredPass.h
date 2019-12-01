#pragma once

#include <vkn/VknFrameBuffer.h>
#include <vkn/PipelineThingy.h>
#include <gfx/RenderObject.h>
namespace idk
{
	struct CameraData;
}
namespace idk::vkn
{
	struct GraphicsState;
	struct RenderStateV2;
	class PipelineManager;
	class VknRenderTarget;

	enum class GBufferBinding
	{
		eAlbedoAmbOcc       ,
		eUvMetallicRoughness,
		eViewPos            ,
		eNormal             ,
		eTangent            ,
	};
	enum class GBufferType
	{
		eMetallic,
		eSpecular
	};

	using EGBufferType = meta::enum_info<GBufferType, meta::enum_pack<GBufferType,
		GBufferType::eMetallic,
		GBufferType::eSpecular
		>>;

	struct DeferredGBuffer
	{
		RscHandle<VknFrameBuffer> gbuffer;
		RscHandle<VknFrameBuffer> accum_buffer; //
		vk::Semaphore RenderCompleteSignal();
		bool Init(ivec2 size);
		RenderPassObj rp_obj;
	private:
		vk::UniqueSemaphore _render_complete;
	};

	struct DeferredPass
	{
		DeferredGBuffer (*_gbuffer)[EGBufferType::size()];
		PipelineManager* _pipeline_manager;
		RenderObject fsq_amb_ro, fsq_light_ro;
		shared_ptr<pipeline_config> ambient_config;
		shared_ptr<pipeline_config> light_config;
		RscHandle<ShaderProgram> fullscreen_quad_vert;
		RscHandle<ShaderProgram> deferred_post_frag[EGBufferType::size()];
		RscHandle<ShaderProgram> deferred_post_ambient  ;
		RscHandle<ShaderProgram> hdr_frag;

		RscHandle<VknFrameBuffer> hdr_buffer;

		RenderPassObj accum_pass, hdr_pass;

		uint32_t _frame_index = 0;
		uint32_t frame_index(uint32_t new_index){ return _frame_index = new_index; }
		uint32_t frame_index()const { return _frame_index; }
		PipelineManager& pipeline_manager(PipelineManager& manager) { return *(_pipeline_manager = &manager); }
		PipelineManager& pipeline_manager()const
		{
			return *_pipeline_manager;
		}
		DeferredGBuffer& GBuffer(GBufferType type) {
			return (*_gbuffer)[EGBufferType::map(type)];
		};
		auto& GBuffers()
		{
			return *_gbuffer;
		}
		//Make sure to call this again if the framebuffer size changed.
		void Init(VknRenderTarget& rt);
		void Init(VknRenderTarget& rt, DeferredGBuffer (&gbuf)[EGBufferType::size()]);
		//void RenderGbufferToTarget(vk::CommandBuffer cmd_buffer, const GraphicsState& graphics_state, RenderStateV2& rs);

		void LightPass(GBufferType type,PipelineThingy& the_interface, const GraphicsState& graphics_state, RenderStateV2& rs, std::optional<std::pair<size_t, size_t>>light_range, bool is_ambient);
		PipelineThingy HdrPass(const GraphicsState& graphics_state, RenderStateV2& rs);
		//PipelineThingy ProcessDrawCalls(const GraphicsState& graphics_state, RenderStateV2& rs, std::optional<std::pair<size_t ,size_t>>light_range);
		void DrawToGBuffers(vk::CommandBuffer cmd_buffer, const GraphicsState& graphics_state, RenderStateV2& rs);
		void DrawToAccum(vk::CommandBuffer cmd_buffer, PipelineThingy  (&accum_stuff)[EGBufferType::size()], const CameraData& camera, RenderStateV2& rs);
		void DrawToRenderTarget(vk::CommandBuffer cmd_buffer, PipelineThingy& fsq_stuff, const CameraData& camera, VknRenderTarget& rt, RenderStateV2& rs);
	};

}