#include "pch.h"
#include "ParticleRenderer.h"
#include <vkn/PipelineThingy.h>
#include <vkn/PipelineBinders.h>
#include <vkn/GraphicsState.h>
#include <gfx/Mesh.h>

namespace idk::vkn
{
	void ParticleRenderer::InitConfig()
	{
		particle_pipeline->buffer_descriptions.emplace_back(
			buffer_desc{
				buffer_desc::binding_info{ {},sizeof(ParticleObj),VertexRate::eInstance },
						{
							buffer_desc::attribute_info
			{
				AttribFormat::eSVec3,2,offsetof(ParticleObj,ParticleObj::position),true
			},
			buffer_desc::attribute_info
			{
				AttribFormat::eSVec1,3,offsetof(ParticleObj,ParticleObj::rotation),true
			},
			buffer_desc::attribute_info
			{
				AttribFormat::eSVec1,4,offsetof(ParticleObj,ParticleObj::size),true
			},
			buffer_desc::attribute_info
			{
				AttribFormat::eSVec4,5,offsetof(ParticleObj,ParticleObj::color),true
			},
						}
			});
		auto test = AttachmentBlendConfig{};
		test.blend_enable = true;
		test.alpha_blend_op = BlendOp::eAdd;
		test.color_blend_op = BlendOp::eAdd;
		test.src_color_blend_factor = BlendFactor::eSrcAlpha;
		test.dst_color_blend_factor = BlendFactor::eOneMinusSrcAlpha;
		test.src_alpha_blend_factor = BlendFactor::eOne;
		test.dst_alpha_blend_factor = BlendFactor::eOneMinusSrcAlpha; //I swear to god idk why this has to be like this, but otherwise, the blend background is just black. eZero didn't work.

		particle_pipeline->attachment_configs =
		{
			test
		};
	}
	void ParticleRenderer::DrawParticles(PipelineThingy& the_interface, const GraphicsState& state, RenderStateV2& rs)
	{
		auto& shared_state = *state.shared_gfx_state;
		ParticleVertexBindings vert_bind;
		StandardMaterialBindings mat_bind;
		mat_bind.SetState(state);
		vert_bind.SetState(state);
		auto num_unique_inst = state.range.inst_particle_end - state.range.inst_particle_begin;
		if (num_unique_inst)
		{
			auto& cam = state.camera;
			the_interface.BindShader(ShaderStage::Vertex, state.renderer_vertex_shaders[VertexShaders::VParticle]);
			auto& particle_render_data = *shared_state.particle_range;
			particle_ro.config = particle_pipeline;
			particle_ro_inst.clear();
			particle_ro_inst.reserve(particle_render_data.size());
			vert_bind.Bind(the_interface);
			for (auto i = state.range.inst_particle_begin;i< state.range.inst_particle_end;++i)
			{
				auto& elem = particle_render_data[i];
				RenderObject part_ro{ particle_ro };

				const auto material = elem.material_instance->material;
				part_ro.material_instance = elem.material_instance;
				part_ro.mesh = Mesh::defaults[MeshType::FSQ];
				part_ro.renderer_req = &particle_vertex_req;
				// bind shader
				the_interface.BindShader(ShaderStage::Fragment, material->_shader_program);
				//TODO bind materials
				mat_bind.Bind(the_interface, part_ro);

				the_interface.BindMeshBuffers(part_ro);
				the_interface.BindAttrib(2, shared_state.particle_buffer.buffer(), 0);


				the_interface.FinalizeDrawCall(particle_ro_inst.emplace_back(std::move(part_ro)), elem.num_elems, elem.elem_offset);

			}
		}
	}
}