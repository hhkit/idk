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
//#pragma optimize("",off)
	void FontRenderer::DrawFont(PipelineThingy& the_interface, const GraphicsState& state, RenderStateV2& rs)
	{
		auto& shared_state = *state.shared_gfx_state;
		
		FontVertexBindings vert_bind;
		vert_bind.SetState(state);
		auto num_unique_inst = state.range.inst_font_end - state.range.inst_font_begin;

		if (num_unique_inst)
		{
			the_interface.BindShader(ShaderStage::Vertex, state.renderer_vertex_shaders[VertexShaders::VFont]);
			the_interface.BindShader(ShaderStage::Fragment, state.renderer_fragment_shaders[FragmentShaders::FFont]);
			auto& character_render_info = *shared_state.characters_data;
			auto& font_render_info = *shared_state.fonts_data;
			font_ro.config = font_pipeline;

			//Reserve buffer size in characters
			font_ro_inst.clear();
			font_ro_inst.reserve(character_render_info.size());

			//Bind camera block
			vert_bind.Bind(the_interface);

			auto i = state.range.inst_font_begin;
			for(auto& elem: font_render_info)
			{
				// bind shader

				//TODO bind font
				vert_bind.BindFont(the_interface, elem);

				//Bind Attribute
				the_interface.BindAttrib(0, shared_state.font_buffer[i].buffer(), 0);

				//Set vertex count
				the_interface.SetVertexCount(s_cast<uint32_t>(elem.coords.size()));

				//Finalize draw call
				the_interface.FinalizeDrawCall(font_ro_inst.emplace_back(std::move(RenderObject{font_ro})));

				++i;
			}
		}
	}
}