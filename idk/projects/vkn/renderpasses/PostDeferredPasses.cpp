#include "pch.h"
#include "PostDeferredPasses.h"
#include <vkn/GraphicsState.h>
#include <gfx/GraphicsSystem.h>


#include <vkn/RenderBindings.inl>
#include <vkn/CameraViewportBindings.h>
#include <vkn/StandardVertexBindings.h>
#include <vkn/StandardMaterialBindings.h>

#include <vkn/binding_util.h>

namespace idk::vkn::bindings
{
	class TransparentBlendBindings : public RenderBindings
	{
	public:
		static inline const std::array<uint32_t, 1> default_attachments{ 0 };
		span<const uint32_t> attachments{ default_attachments };
		void Bind(RenderInterface& context)
		{
			AttachmentBlendConfig test{};
			test.blend_enable = true;
			test.alpha_blend_op = BlendOp::eAdd;
			test.color_blend_op = BlendOp::eAdd;
			test.src_color_blend_factor = BlendFactor::eSrcAlpha;
			test.dst_color_blend_factor = BlendFactor::eOneMinusSrcAlpha;
			test.src_alpha_blend_factor = BlendFactor::eOne;
			test.dst_alpha_blend_factor = BlendFactor::eOneMinusSrcAlpha; //I swear to god idk why this has to be like this, but otherwise, the blend background is just black. eZero didn't work.
			for (auto attachment_idx: attachments)
			{
				context.SetBlend(attachment_idx, test);
			}
		}
	};
	using ParticleBindings = CombinedBindings<TransparentBlendBindings,CameraViewportBindings, VertexShaderBinding, ParticleVertexBindings, StandardMaterialFragBindings, StandardMaterialBindings>;
}


namespace idk::vkn::renderpasses
{

	using ParticleBindingLogic = GenericDrawSet<bindings::ParticleBindings, ParticleDrawSet>;

	using ParticlePass = PassSetPair<BasicRenderTargetPass, ParticleBindingLogic>;




	void ParticleRenderer::Init()
	{
	}
	std::pair<FrameGraphResource, FrameGraphResource> ParticleRenderer::AddPass(FrameGraph& graph, const GraphicsState& graphics_state,FrameGraphResource color, FrameGraphResource depth)
	{
		bindings::ParticleBindings bindings;

		bindings.for_each_binder<>(
			bindings::BinderForward<bindings::SetStateTest>{},
			graphics_state.camera);
		auto& vtx_shader_bind = bindings.Get<bindings::VertexShaderBinding>();
		vtx_shader_bind.vertex_shader = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VParticle];
		auto& mat_shader_bind = bindings.Get<bindings::StandardMaterialBindings>();
		mat_shader_bind.p_material_instances = &graphics_state.material_instances;

		index_span particle_idx{ graphics_state.range.inst_particle_begin,graphics_state.range.inst_particle_end};

		ParticleDrawSet  logic   {particle_idx.to_span(*graphics_state.shared_gfx_state->particle_range),graphics_state.shared_gfx_state->particle_buffer };
		ParticleBindingLogic binding_and_logic{bindings,logic};
		auto& pass =graph.addRenderPass<ParticlePass>("Particle Pass", binding_and_logic,std::nullopt, std::nullopt, color, depth).RenderPass(); 

		return std::pair<FrameGraphResource, FrameGraphResource>(pass.color_rsc,pass.depth_rsc);
	}
}
