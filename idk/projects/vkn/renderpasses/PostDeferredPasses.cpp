#include "pch.h"
#include "PostDeferredPasses.h"
#include <vkn/GraphicsState.h>
#include <gfx/GraphicsSystem.h>


#include <vkn/RenderBindings.inl>
#include <vkn/CameraViewportBindings.h>
#include <vkn/StandardVertexBindings.h>
#include <vkn/StandardFragmentBinding.h>
#include <vkn/StandardMaterialBindings.h>
#include <vkn/TransparentBlendBindings.h>

#include <res/ResourceHandle.inl>
#include <vkn/VknFontAtlas.h>

#include <vkn/binding_util.h>

namespace idk::vkn::bindings
{
	using ParticleBindings = CombinedBindings<TransparentBlendBindings,CameraViewportBindings, VertexShaderBinding, ParticleVertexBindings, StandardMaterialFragBindings, StandardMaterialBindings>;
}
namespace idk::vkn::bindings
{
	struct FontVertexBindings : RenderBindings
	{
		//const GraphicsState* _state;
		//const GraphicsState& State();
		mat4 view_trf, proj_trf, obj_trf;
		vec4 color;
		void SetState(const GraphicsState& vstate);
		void SetState(const CameraData& camera);

		void Bind(RenderInterface& the_interface)override;
		void BindFont(RenderInterface& the_interface, const FontRenderData& dc)override;


	};
	using TextMeshBindings = CombinedBindings<TransparentBlendBindings, CameraViewportBindings, VertexShaderBinding, FragmentShaderBinding, FontVertexBindings >;




	void FontVertexBindings::SetState(const GraphicsState& vstate)
	{
		auto& cam = vstate.camera;
		SetState(cam);
	}

	void FontVertexBindings::SetState(const CameraData& cam)
	{
		view_trf = cam.view_matrix;
		proj_trf = cam.projection_matrix;
	}

	void FontVertexBindings::Bind(RenderInterface& the_interface)
	{
		//map back into z: (0,1)
		mat4 projection_trf = mat4{ 1,0,0,0,
							0,1,0,0,
							0,0,0.5f,0.5f,
							0,0,0,1
		}*proj_trf;//map back into z: (0,1)
		mat4 block[] = { proj_trf };
		the_interface.BindUniform("CameraBlock", 0, hlp::to_data(block));
	}

	void FontVertexBindings::BindFont(RenderInterface& the_interface, const FontRenderData& dc)
	{
		mat4 obj_trfm = view_trf * dc.transform;
		mat4 obj_ivt = obj_trfm.inverse().transpose();
		mat4 block2[] = { obj_trfm };
		the_interface.BindUniform("ObjectMat4Block", 0, hlp::to_data(block2));
		vec4 block3[] = { dc.color.as_vec4 };
		the_interface.BindUniform("FontBlock", 0, hlp::to_data(block3));
		the_interface.BindUniform("tex", 0, *dc.atlas.as<VknFontAtlas>().texture);
	}

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




	using TextMeshBindingLogic = GenericDrawSet<bindings::TextMeshBindings, TextMeshDrawSet>;

	using TextMeshPass = PassSetPair<BasicRenderTargetPass, TextMeshBindingLogic>;


	std::pair<FrameGraphResource, FrameGraphResource> TextMeshRenderer::AddPass(FrameGraph& graph, const GraphicsState& state, FrameGraphResource color, FrameGraphResource depth)
	{
		bindings::TextMeshBindings bindings;

		bindings.for_each_binder<>(
			bindings::BinderForward<bindings::SetStateTest>{},
			state.camera);
		auto& vtx_shader_bind = bindings.Get<bindings::VertexShaderBinding>();
		auto& frg_shader_bind = bindings.Get<bindings::FragmentShaderBinding>();

		;

		vtx_shader_bind.vertex_shader = Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VertexShaders::VFont];
		frg_shader_bind.fragment_shader= Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FragmentShaders::FFont];
		index_span particle_idx{ state.range.inst_particle_begin,state.range.inst_particle_end };

		TextMeshDrawSet  logic{ *state.shared_gfx_state->fonts_data,state.shared_gfx_state->font_buffer};
		TextMeshBindingLogic binding_and_logic{ bindings,logic };
		auto& pass = graph.addRenderPass<TextMeshPass>("Text Mesh Pass", binding_and_logic, std::nullopt, std::nullopt, color, depth).RenderPass();

		return std::pair<FrameGraphResource, FrameGraphResource>(pass.color_rsc, pass.depth_rsc);
	}
}
