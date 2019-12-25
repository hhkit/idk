#include "pch.h"
#include "CanvasRenderer.h"
#include <vkn/PipelineThingy.h>
#include <vkn/PipelineBinders.h>
#include <vkn/GraphicsState.h>
#include <gfx/Mesh.h>

#include <vkn/FrameRenderer.h>
#include <res/ResourceHandle.inl>

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

		canvas_pipeline->attachment_configs =
		{
			test
		};
		canvas_pipeline->depth_write = false;
		canvas_pipeline->depth_test = false;

		canvas_pipeline2->attachment_configs =
		{
			test
		};

		canvas_pipeline2->depth_write = false;
		canvas_pipeline2->depth_test = false;
	}
	void CanvasRenderer::DrawCanvas(size_t& canvas_count,PipelineThingy& the_interface, const PostRenderData& state, [[maybe_unused]]RenderStateV2& rs, const vector<UIRenderObject>& canvas_data)
	{
		auto& shared_state = *state.shared_gfx_state;

		CanvasVertexBindings vert_bind;

		if (canvas_data.size())
		{
			the_interface.BindShader(ShaderStage::Vertex, state.renderer_vertex_shaders[VertexShaders::VUi]);
			
			auto& font_render_info = shared_state.ui_text_buffer_pos;
			auto& font_uv_info = shared_state.ui_text_buffer_uv;
			auto& font_raw_data = *shared_state.ui_text_range;
			canvas_ro_inst.clear();
			canvas_ro_inst.reserve(canvas_data.size());

			canvas_ro_inst2.clear();
			canvas_ro_inst2.reserve(font_render_info.size());

			//size_t i = state.range.inst_font_begin;
			//size_t accum_size = 0;
			//auto& canvas_range = canvas_range_data[canvas_count];
			//auto i = canvas_range.inst_font_begin;
			auto& i = canvas_count;
			for (auto& ui_canvas : canvas_data)
			{
				RenderObject canvas_ui_ro;
				std::visit([&](const auto& data)
				{
					using T = std::decay_t<decltype(data)>;
					if constexpr (std::is_same_v<T, ImageData>)
					{
						the_interface.BindShader(ShaderStage::Fragment, ui_canvas.material->material->_shader_program);

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
						the_interface.BindShader(ShaderStage::Fragment, ui_canvas.material->material->_shader_program);

						canvas_ui_ro.config = canvas_pipeline2;
						canvas_ui_ro.material_instance = ui_canvas.material;
						auto& elem = font_render_info[i];
						auto& uv = font_uv_info[i];
						auto& v_size = font_raw_data[i];
						//TODO bind materials
						vert_bind.BindCanvas(the_interface, data, ui_canvas);
						
						//the_interface.BindMeshBuffers(f_ro);
						the_interface.BindAttrib(0, elem.buffer(), 0);
						the_interface.BindAttrib(1, uv.buffer()  , 0);

						the_interface.SetVertexCount(s_cast<uint32_t>(v_size.num_elems));

						the_interface.FinalizeDrawCall(canvas_ro_inst2.emplace_back(std::move(canvas_ui_ro)));

						++i;				
					}
				}, ui_canvas.data);
			}
		}
	}
}