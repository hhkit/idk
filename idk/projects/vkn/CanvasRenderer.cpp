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
	void CanvasRenderer::DrawCanvas(size_t& ui_elem_count, size_t& text_count, PipelineThingy& the_interface, const PostRenderData& state, [[maybe_unused]]RenderStateV2& rs, const vector<UIRenderObject>& canvas_data)
	{
		auto& shared_state = *state.shared_gfx_state;

		const auto& test = static_cast<const GraphicsState&>(static_cast<const CoreGraphicsState&>(state));

		CanvasVertexBindings vert_bind;
		StandardMaterialBindings mat_bind;
		mat_bind.SetState(test);

		auto renderer_vertex_shaders = state.shared_gfx_state->renderer_vertex_shaders;

		if (canvas_data.size())
		{
			the_interface.BindShader(ShaderStage::Vertex, renderer_vertex_shaders[VertexShaders::VUi]);
			
			auto& ui_buffer_pos = shared_state.ui_buffer_pos;
			auto& ui_buffer_uv = shared_state.ui_buffer_uv;
			auto& ui_buffer_color = shared_state.ui_buffer_color;
			auto& font_raw_data = *shared_state.ui_text_range;
			canvas_ro_inst.clear();
			canvas_ro_inst.reserve(canvas_data.size());

			canvas_ro_inst2.clear();
			canvas_ro_inst2.reserve(ui_buffer_pos.size());

			//size_t i = state.range.inst_font_begin;
			//size_t accum_size = 0;
			//auto& canvas_range = canvas_range_data[canvas_count];
			//auto i = canvas_range.inst_font_begin;
			auto& i = ui_elem_count;
			for (auto& ui_canvas : canvas_data)
			{
				RenderObject canvas_ui_ro;
				std::visit([&](const auto& data)
				{
					using T = std::decay_t<decltype(data)>;
					if constexpr (std::is_same_v<T, ImageData>)
					{
						state.shared_gfx_state->material_instances.emplace(ui_canvas.material, ProcessedMaterial{ ui_canvas.material });

						canvas_ui_ro.config = canvas_pipeline;
						canvas_ui_ro.material_instance = ui_canvas.material;
						canvas_ui_ro.mesh = Mesh::defaults[MeshType::FSQ];
						canvas_ui_ro.renderer_req = &canvas_vertex_req;

						if (!ui_canvas.material) // invalid material!
							canvas_ui_ro.material_instance = UISystem::default_material_inst;

						the_interface.BindShader(ShaderStage::Fragment, canvas_ui_ro.material_instance->material->_shader_program);

						mat_bind.Bind(the_interface, canvas_ui_ro);
						vert_bind.BindCanvas(the_interface, data, ui_canvas);
						the_interface.BindMeshBuffers(canvas_ui_ro);
						the_interface.BindAttrib(2, ui_buffer_color[i].buffer(), 0);

						the_interface.FinalizeDrawCall(canvas_ro_inst.emplace_back(std::move(canvas_ui_ro)));
					}
					else
					{
						the_interface.BindShader(ShaderStage::Fragment, state.shared_gfx_state->renderer_fragment_shaders[FragmentShaders::FFont]);

						canvas_ui_ro.config = canvas_pipeline2;
						canvas_ui_ro.material_instance = ui_canvas.material;
						auto& elem = ui_buffer_pos[i];
						auto& uv = ui_buffer_uv[i];
						auto& v_size = font_raw_data[text_count];
						//TODO bind materials
						vert_bind.BindCanvas(the_interface, data, ui_canvas);
						
						//the_interface.BindMeshBuffers(f_ro);
						the_interface.BindAttrib(0, elem.buffer(), 0);
						the_interface.BindAttrib(1, uv.buffer()  , 0);
						the_interface.BindAttrib(2, ui_buffer_color[i].buffer(), 0);

						the_interface.SetVertexCount(s_cast<uint32_t>(v_size.num_elems));

						the_interface.FinalizeDrawCall(canvas_ro_inst2.emplace_back(std::move(canvas_ui_ro)));		

						++text_count;
					}
				}, ui_canvas.data);
				++i;
			}
		}
	}
}