#include "pch.h"
#include "CanvasRenderer.h"
#include <vkn/PipelineThingy.h>
#include <vkn/PipelineBinders.h>
#include <vkn/GraphicsState.h>
#include <gfx/Mesh.h>

#include <vkn/FrameRenderer.h>

namespace idk::vkn
{
	void CanvasRenderer::InitConfig()
	{
		canvas_pipeline->buffer_descriptions.emplace_back(
			buffer_desc{
				buffer_desc::binding_info{ {},sizeof(vec3),VertexRate::eVertex },
						{
							buffer_desc::attribute_info
						{
							AttribFormat::eSVec2,0,0,true
						}
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

		canvas_pipeline2->attachment_configs =
		{
			test
		};
	}
//#pragma optimize("",off)
	void CanvasRenderer::DrawCanvas(PipelineThingy& the_interface, const PostRenderData& state, RenderStateV2& rs, const vector<UIRenderObject>& canvas_data)
	{
		auto& shared_state = *state.shared_gfx_state;

		CanvasVertexBindings vert_bind;

		if (canvas_data.size())
		{
			the_interface.BindShader(ShaderStage::Vertex, state.renderer_vertex_shaders[VertexShaders::VUi]);
			auto& character_render_info = *shared_state.ui_text_data;
			auto& font_render_info = shared_state.ui_text_buffer_pos;
			auto& font_uv_info = shared_state.ui_text_buffer_uv;
			auto& font_raw_data = *shared_state.ui_text_range;
			canvas_ro_inst.clear();
			canvas_ro_inst.reserve(canvas_data.size());

			//size_t i = state.range.inst_font_begin;
			auto i = 0;
			
			for (auto& ui_canvas : canvas_data)
			{
				RenderObject canvas_ui_ro;

				the_interface.BindShader(ShaderStage::Fragment, ui_canvas.material->material->_shader_program);	
				
				std::visit([&](const auto& data)
				{
					using T = std::decay_t<decltype(data)>;
					if constexpr (std::is_same_v<T, ImageData>)
					{
						
						canvas_ui_ro.config = canvas_pipeline;
						canvas_ui_ro.material_instance = ui_canvas.material;
						canvas_ui_ro.mesh = Mesh::defaults[MeshType::FSQ];
						canvas_ui_ro.renderer_req = &canvas_vertex_req;

						vert_bind.BindCanvas(the_interface, data, ui_canvas);

						the_interface.BindMeshBuffers(canvas_ui_ro);

						the_interface.FinalizeDrawCall(canvas_ro_inst.emplace_back(std::move(canvas_ui_ro)));
					}
					else
					{
						canvas_ui_ro.config = canvas_pipeline2;
						auto& elem = font_render_info[i];
						auto& uv = font_uv_info[i];

						//canvas_ui_ro.renderer_req = &canvas_vertex_req;
						canvas_ui_ro.material_instance = ui_canvas.material;

						//TODO bind materials
						vert_bind.BindCanvas(the_interface, data, ui_canvas);

						//the_interface.BindMeshBuffers(f_ro);
						the_interface.BindAttrib(0, elem.buffer(), 0);
						the_interface.BindAttrib(1, uv.buffer(), 0);

						the_interface.SetVertexCount(s_cast<uint32_t>(data.coords.size()));

						the_interface.FinalizeDrawCall(canvas_ro_inst.emplace_back(std::move(canvas_ui_ro)));

						++i;
						
					}
				}, ui_canvas.data);
			}
		}
	}
}