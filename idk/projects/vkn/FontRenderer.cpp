#include "pch.h"
#include "FontRenderer.h"
#include <vkn/PipelineThingy.h>
#include <vkn/PipelineBinders.h>
#include <vkn/GraphicsState.h>
#include <gfx/Mesh.h>

namespace idk::vkn
{
	void FontRenderer::InitConfig()
	{
		//font_pipeline->buffer_descriptions.emplace_back(
		//	buffer_desc{
		//		buffer_desc::binding_info{ {},sizeof(ParticleObj),VertexRate::eInstance },
		//				{
		//					/*buffer_desc::attribute_info
		//	{
		//		AttribFormat::eSVec4,2,offsetof(ParticleObj,ParticleObj::position),true
		//	},
		//	buffer_desc::attribute_info
		//	{
		//		AttribFormat::eSVec1,3,offsetof(ParticleObj,ParticleObj::rotation),true
		//	},
		//	buffer_desc::attribute_info
		//	{
		//		AttribFormat::eSVec1,4,offsetof(ParticleObj,ParticleObj::size),true
		//	},
		//	buffer_desc::attribute_info
		//	{
		//		AttribFormat::eSVec4,5,offsetof(ParticleObj,ParticleObj::color),true
		//	},*/
		//				}
		//	});
		auto test = AttachmentBlendConfig{};
		test.blend_enable = true;
		test.alpha_blend_op = BlendOp::eAdd;
		test.color_blend_op = BlendOp::eAdd;
		test.src_color_blend_factor = BlendFactor::eSrcAlpha;
		test.dst_color_blend_factor = BlendFactor::eOneMinusSrcAlpha;
		test.src_alpha_blend_factor = BlendFactor::eOne;
		test.dst_alpha_blend_factor = BlendFactor::eOneMinusSrcAlpha; //I swear to god idk why this has to be like this, but otherwise, the blend background is just black. eZero didn't work.

		font_pipeline->attachment_configs =
		{
			test
		};
	}
#pragma optimize("",off)
	void FontRenderer::DrawFont(PipelineThingy& the_interface, const GraphicsState& state, RenderStateV2& rs)
	{
		auto& shared_state = *state.shared_gfx_state;
		
		FontVertexBindings vert_bind;
		//StandardMaterialBindings mat_bind;
		//mat_bind.SetState(state);
		vert_bind.SetState(state);
		auto num_unique_inst = state.range.inst_font_end - state.range.inst_font_begin;

		if (num_unique_inst)
		{
			auto& cam = state.camera;
			the_interface.BindShader(ShaderStage::Vertex, state.renderer_vertex_shaders[VertexShaders::VFont]);
			the_interface.BindShader(ShaderStage::Fragment, state.renderer_fragment_shaders[FragmentShaders::FFont]);
			auto& character_render_info = *shared_state.characters_data;
			auto& font_render_info = *shared_state.fonts_data;
			auto& font_raw_data = *shared_state.font_range;
			font_ro.config = font_pipeline;
			font_ro_inst.clear();
			font_ro_inst.reserve(character_render_info.size());
			vert_bind.Bind(the_interface);

			//size_t i = state.range.inst_font_begin;
			for(auto& elem: font_render_info)
			{
				auto i = state.range.inst_font_begin;
				//size_t list_start_size = i, character_list_size = list_start_size + (elem.characters.size());
				//for (size_t j = state.range.inst_font_begin; j < state.range.inst_font_end; ++j)
				{
					auto& raw_elem = font_raw_data[i];
					RenderObject f_ro{ font_ro };

					//f_ro.mesh = Mesh::defaults[MeshType::FSQ];
					//f_ro.renderer_req = &font_vertex_req;
					// bind shader

					//TODO bind materials
					//mat_bind.Bind(the_interface, f_ro);
					vert_bind.BindFont(the_interface, elem);

					//the_interface.BindMeshBuffers(f_ro);
					the_interface.BindAttrib(0, shared_state.font_buffer.buffer(), 0);
					the_interface.SetVertexCount(raw_elem.num_elems);

					the_interface.FinalizeDrawCall(font_ro_inst.emplace_back(std::move(f_ro)), raw_elem.num_elems, raw_elem.elem_offset);
				}

				++i;
			}
		}
	}
}