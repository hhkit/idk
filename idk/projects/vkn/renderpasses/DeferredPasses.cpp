#include "pch.h"
#include "DeferredPasses.h"
#include <vkn/VknRenderTarget.h>
#include <res/ResourceHandle.inl>
#include <vkn/VulkanMesh.h>
#include <vkn/GraphicsState.h>
#include <vkn/RenderStateV2.h>
#include <gfx/GraphicsSystem.h>
#include <vkn/BufferHelpers.inl>
#include <vkn/FrameGraph.h>
namespace idk::vkn::renderpasses
{
	using Context_t = PassUtil::Context_t;

	TextureDescription CreateTextureInfo(FrameGraphBuilder& builder, string_view name, vk::Format format, vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits flag = vk::ImageAspectFlagBits::eColor, std::optional<RscHandle<VknTexture>> target = {}, uvec2 size = uvec2{ 1920,1080 });
	FrameGraphResourceMutable CreateGBuffer(FrameGraphBuilder& builder, string_view name, vk::Format format, vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits flag = vk::ImageAspectFlagBits::eColor, std::optional<RscHandle<VknTexture>> target = {});
	void BindMesh(Context_t context, const renderer_attributes& req, VulkanMesh& mesh);

	TextureDescription CreateTextureInfo(FrameGraphBuilder& builder, string_view name, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlagBits flag, std::optional<RscHandle<VknTexture>> target, uvec2 size)
	{
		return TextureDescription
		{
			.name = name,//string_view name);
			.size = size,//ivec2 size);
			.format = format,//vk::Format format);
			.aspect = flag,//vk::ImageAspectFlags aspect);
						   //vk::ImageType type = vk::ImageType::e2D);
						   //uint32_t layer_count = 1);
						   //vk::ImageTiling tiling_format);
						   .usage = usage,
						   .actual_rsc = target,
		};

	}

	FrameGraphResourceMutable CreateGBuffer(FrameGraphBuilder& builder, string_view name, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlagBits flag, std::optional<RscHandle<VknTexture>> target)
	{
		return builder.write(builder.CreateTexture(CreateTextureInfo(builder, name, format, usage, flag, target)));

	}

	void BindMesh(Context_t context, const renderer_attributes& req, VulkanMesh& mesh)
	{
		for (auto& [attrib, buffer] : mesh.Buffers())
		{
			if (buffer.buffer())
			{
				auto index_itr = req.mesh_requirements.find(attrib);
				if (index_itr != req.mesh_requirements.end())
				{
					context.BindVertexBuffer(index_itr->second, *buffer.buffer(), buffer.offset);
				}
			}
		}
		auto idx_buffer = mesh.GetIndexBuffer();
		if (idx_buffer)
		{
			context.BindIndexBuffer(*idx_buffer->buffer(), idx_buffer->offset, mesh.IndexType());
		}
	}

	GBufferPass::GBufferPass(FrameGraphBuilder& builder, RscHandle<VknRenderTarget> rt, bool clear_depth)
	{
		gbuffer_rscs[0] = CreateGBuffer(builder, "AlbedoAmbOcc", vk::Format::eR8G8B8A8Unorm);
		gbuffer_rscs[1] = CreateGBuffer(builder, "eUvMetallicRoughness", vk::Format::eR8G8B8A8Unorm);
		gbuffer_rscs[2] = CreateGBuffer(builder, "ViewPos", vk::Format::eR16G16B16A16Sfloat);
		gbuffer_rscs[3] = CreateGBuffer(builder, "Normal", vk::Format::eR8G8B8A8Unorm);
		gbuffer_rscs[4] = CreateGBuffer(builder, "Tangent", vk::Format::eR8G8B8A8Unorm);
		gbuffer_rscs[5] = CreateGBuffer(builder, "Emissive", vk::Format::eR8G8B8A8Srgb);
		depth_rsc = CreateGBuffer(builder, "GDepth", vk::Format::eD32Sfloat, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth, RscHandle<VknTexture>{rt->GetDepthBuffer()});
		uint32_t index = 0;
		for (auto& gbuffer_rsc : gbuffer_rscs)
		{
			builder.set_output_attachment(gbuffer_rsc, index++, AttachmentDescription
				{
					vk::AttachmentLoadOp::eClear,//vk::AttachmentLoadOp load_op;
					vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp stencil_store_op;
					vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
					vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
					vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
					vk::ImageSubresourceRange
					{
						vk::ImageAspectFlagBits::eColor,0,1,0,1
					},//vk::ImageSubresourceRange sub_resource_range{};
					vk::ClearColorValue{},//std::optional<vk::ClearValue> clear_value;
					//std::optional<vk::Format> format{};
					//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
					//vk::ComponentMapping mapping{};
				}
			);
		}
		builder.set_depth_stencil_attachment(depth_rsc, AttachmentDescription
			{
				(clear_depth)?vk::AttachmentLoadOp::eClear: vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
				vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp stencil_store_op;
				vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
				vk::ImageSubresourceRange
				{
					vk::ImageAspectFlagBits::eDepth,0,1,0,1
				},//vk::ImageSubresourceRange sub_resource_range{};
				vk::ClearDepthStencilValue{},//std::optional<vk::ClearValue> clear_value;
				//std::optional<vk::Format> format{};
				//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
				//vk::ComponentMapping mapping{};
			});
	}
	void GBufferPass::Execute(FrameGraphDetail::Context_t context, BaseDrawSet& draw_set)
	{
		context.DebugLabel(RenderTask::LabelLevel::eWhole, "FG: Gbuffer pass");
		std::array description{
			buffer_desc
		{
			buffer_desc::binding_info{ std::nullopt,sizeof(mat4) * 2,VertexRate::eInstance },
		{ buffer_desc::attribute_info{ AttribFormat::eMat4,4,0,true },
		buffer_desc::attribute_info{ AttribFormat::eMat4,8,sizeof(mat4),true }
		}
		}
		};
		context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VNormalMesh]);
		context.SetBufferDescriptions(description);
		uint32_t i = 0;
		for ([[maybe_unused]] auto& buffer : gbuffer_rscs)
		{
			context.SetBlend(i);
			context.SetClearColor(i, idk::color{ 0,0,0,0 });
			++i;
		}
		context.SetClearDepthStencil(1.0f);
		//context.SetViewport(gfx_state.camera.viewport);
		//context.SetScissors(gfx_state.camera.viewport);
		//auto mesh_range = index_span{ gfx_state.range.inst_mesh_render_begin,gfx_state.range.inst_mesh_render_end };
		//const renderer_attributes* prev_req = {};
		draw_set.Render(context);
		/*
		for (auto& ro : mesh_range.to_span(*gfx_state.shared_gfx_state->instanced_ros))
		{
			auto& mesh = ro.mesh.as<VulkanMesh>();
			auto& req = *ro.renderer_req;
			if (&req != prev_req)
			{
				context.BindVertexBuffer(req.instanced_requirements.find(vtx::InstAttrib::_enum::ModelTransform)->second, gfx_state.shared_gfx_state->inst_mesh_render_buffer.buffer(), 0);
				prev_req = &req;
			}
			auto idx_buffer = mesh.GetIndexBuffer();

			auto mat_info = ro.material_instance->get_cache();
			auto& frag = ro.material_instance->material->_shader_program;
			if (!frag)
				continue;
			context.BindShader(ShaderStage::Fragment, ro.material_instance->material->_shader_program);
			context.BindUniform("CameraBlock", 0, to_data(gfx_state.camera.projection_matrix));
			{
				auto itr = mat_info.uniforms.begin();
				auto end = mat_info.uniforms.end();
				while (itr != end)
				{
					if (mat_info.IsUniformBlock(itr))
					{

						context.BindUniform(itr->first, 0, mat_info.GetUniformBlock(itr));
					}
					else if (mat_info.IsImageBlock(itr))
					{
						auto img_block = mat_info.GetImageBlock(itr);
						auto name = string_view{ itr->first };
						name = name.substr(0, name.find_first_of('['));
						for (size_t img_index = 0; img_index < img_block.size(); ++img_index)
						{
							context.BindUniform(name, img_index, img_block[img_index].as<VknTexture>());
						}
					}
					++itr;
				}
			}
			BindMesh(context, req, mesh);
			if (idx_buffer)
			{
				context.BindIndexBuffer(*idx_buffer->buffer(), idx_buffer->offset, mesh.IndexType());
				context.DrawIndexed(mesh.IndexCount(), ro.num_instances, 0, 0, ro.instanced_index);
			}
			else
			{
				context.Draw(mesh.IndexCount(), ro.num_instances, 0, ro.instanced_index);

			}
		}*/
	}



	HdrPass::HdrPass(FrameGraphBuilder& builder, AccumPass& accum_, RscHandle<VknRenderTarget>, FullRenderData& rd) :PassUtil{ rd }, accum{ accum_ }
	{
		hdr_rsc = CreateGBuffer(builder, "HDR", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor);
		depth_att = CreateGBuffer(builder, "Depth", vk::Format::eD16Unorm, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth);
		builder.set_output_attachment(hdr_rsc, 0, AttachmentDescription
			{
				vk::AttachmentLoadOp::eClear,//vk::AttachmentLoadOp load_op;
				vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp stencil_store_op;
				vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
				vk::ImageSubresourceRange
				{
					vk::ImageAspectFlagBits::eColor,0,1,0,1
				},//vk::ImageSubresourceRange sub_resource_range{};
				vk::ClearColorValue{},//std::optional<vk::ClearValue> clear_value;
				//std::optional<vk::Format> format{};
				//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
				//vk::ComponentMapping mapping{};
			}
		);
		auto derp1 = builder.read(accum.accum_rsc);
		auto derp2 = builder.read(accum.depth_rsc);
		auto derp3 = builder.read(accum.accum_rsc);
		auto derp4 = builder.read(accum.depth_rsc);
		builder.set_input_attachment(accum_att = derp1, 0, AttachmentDescription
			{
				vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
				vk::ImageSubresourceRange
				{
					vk::ImageAspectFlagBits::eColor,0,1,0,1
				},//vk::ImageSubresourceRange sub_resource_range{};
				//std::optional<vk::ClearValue> clear_value;
				//std::optional<vk::Format> format{};
				//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
				//vk::ComponentMapping mapping{};
			});
		builder.set_input_attachment(derp2, 1, AttachmentDescription
			{
				vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
				vk::ImageSubresourceRange
				{
					vk::ImageAspectFlagBits::eDepth,0,1,0,1
				},//vk::ImageSubresourceRange sub_resource_range{};
				//std::optional<vk::ClearValue> clear_value;
				//std::optional<vk::Format> format{};
				//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
				//vk::ComponentMapping mapping{};
			});

		builder.set_input_attachment(derp3, 2, AttachmentDescription
			{
				vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
				vk::ImageSubresourceRange
				{
					vk::ImageAspectFlagBits::eColor,0,1,0,1
				},//vk::ImageSubresourceRange sub_resource_range{};
				//std::optional<vk::ClearValue> clear_value;
				//std::optional<vk::Format> format{};
				//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
				//vk::ComponentMapping mapping{};
			});
		builder.set_input_attachment(derp4, 3, AttachmentDescription
			{
				vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
				vk::ImageSubresourceRange
				{
					vk::ImageAspectFlagBits::eDepth,0,1,0,1
				},//vk::ImageSubresourceRange sub_resource_range{};
				//std::optional<vk::ClearValue> clear_value;
				//std::optional<vk::Format> format{};
				//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
				//vk::ComponentMapping mapping{};
			});
		builder.set_depth_stencil_attachment(depth_att, AttachmentDescription
			{
				vk::AttachmentLoadOp::eClear,//vk::AttachmentLoadOp load_op;
				vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp stencil_store_op;
				vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
				vk::ImageSubresourceRange
				{
					vk::ImageAspectFlagBits::eDepth,0,1,0,1
				},//vk::ImageSubresourceRange sub_resource_range{};
				vk::ClearDepthStencilValue{},//std::optional<vk::ClearValue> clear_value;
				//std::optional<vk::Format> format{};
				//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
				//vk::ComponentMapping mapping{};
			});
	}
	void HdrPass::Execute(FrameGraphDetail::Context_t context)
	{
		context.SetUboManager(this->render_data.rs_state->ubo_manager);
		auto& gfx_state = this->render_data.GetGfxState();
		context.DebugLabel(RenderTask::LabelLevel::eWhole, "FG: HDR Pass");
		context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);
		if (!hdr_shader)
		{
			auto frag_opt = Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/deferred_hdr.frag", false);
			if (frag_opt)
				hdr_shader = *frag_opt;
		}

		context.BindShader(ShaderStage::Fragment, hdr_shader);
		{
			uint32_t i = 0;

			AttachmentBlendConfig blend{};
			blend.blend_enable = true;
			blend.dst_color_blend_factor = BlendFactor::eOne;
			blend.src_color_blend_factor = BlendFactor::eOne;
			blend.color_blend_op = BlendOp::eAdd;
			blend.alpha_blend_op = BlendOp::eMax;
			blend.dst_alpha_blend_factor = BlendFactor::eOne;
			blend.src_alpha_blend_factor = BlendFactor::eOne;
			context.SetBlend(i);
			context.SetClearColor(i, idk::color{ 0,0,0,0 });
			context.SetClearDepthStencil(1.0f);
			++i;
		}
		context.SetViewport(gfx_state.camera.viewport);
		context.SetScissors(gfx_state.camera.viewport);

		context.SetCullFace({});
		//context.SetDepthTest(false);

		auto& mesh = Mesh::defaults[MeshType::FSQ].as<VulkanMesh>();
		BindMesh(context, fsq_requirements, mesh);

		//DrawFSQ
		context.DrawIndexed(mesh.IndexCount(), 1, 0, 0, 0);
	}




	CubeClearPass::CubeClearPass(FrameGraphBuilder& builder, FullRenderData& frd) : PassUtil{ frd }
	{
		auto& gfx_state = this->render_data.GetGfxState();
		auto color_att = CreateGBuffer(builder, "ClearColor", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor);
		render_target = color_att;
		std::array<float, 4> clear_color{};
		auto& cam_clear = gfx_state.camera.clear_data;
		auto dc_val = meta::IndexOf<std::remove_cvref_t<decltype(cam_clear)>, DontClear>::value;
		auto do_val = meta::IndexOf<std::remove_cvref_t<decltype(cam_clear)>, DepthOnly>::value;
		builder.set_output_attachment(color_att, 0,
			AttachmentDescription
			{
					(dc_val != cam_clear.index() && do_val != cam_clear.index()) ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
					vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp store_op;
					vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
					vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
					vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
					vk::ImageSubresourceRange
					{
						vk::ImageAspectFlagBits::eColor,0,1,0,1
					},//vk::ImageSubresourceRange sub_resource_range{};
					vk::ClearColorValue{clear_color}//std::optional<vk::ClearValue> clear_value;
					//std::optional<vk::Format> format{};
					//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
					//vk::ComponentMapping mapping{};
			}
		);
	}
	void CubeClearPass::Execute(Context_t context)
	{
		context.SetUboManager(this->render_data.rs_state->ubo_manager);
		auto& gfx_state = this->render_data.GetGfxState();
		context.DebugLabel(RenderTask::LabelLevel::eWhole, "FG: Cube Clear");
		context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);

		{
			size_t i = 0;
			auto& cam_clear = gfx_state.camera.clear_data;
			auto color_index = meta::IndexOf<std::remove_cvref_t<decltype(cam_clear)>, color>::value;
			//auto cube_index = meta::IndexOf<std::remove_cvref_t<decltype(cam_clear)>, RscHandle<CubeMap>>::value;
			color col = {};
			if (color_index == cam_clear.index())
			{
				col = std::get<color>(cam_clear);
			}

			AttachmentBlendConfig blend{};
			blend.blend_enable = true;
			blend.dst_color_blend_factor = BlendFactor::eOne;
			blend.src_color_blend_factor = BlendFactor::eOne;
			blend.color_blend_op = BlendOp::eAdd;
			blend.alpha_blend_op = BlendOp::eMax;
			blend.dst_alpha_blend_factor = BlendFactor::eOne;
			blend.src_alpha_blend_factor = BlendFactor::eOne;
			context.SetBlend(i);
			context.SetClearColor(i, col);
			context.SetClearDepthStencil(1.0f);
			++i;
		}
		context.SetViewport(gfx_state.camera.viewport);
		context.SetScissors(gfx_state.camera.viewport);

		//auto& light_indices = gfx_state.active_lights;
		//vector<LightData> lights;
		//vector<VknTextureView> shadow_maps;
		//lights.reserve(8);
		//shadow_maps.reserve(8);
		//FakeMat4 pbr_trf = gfx_state.camera.view_matrix.inverse();
		//auto& mesh = Mesh::defaults[MeshType::FSQ].as<VulkanMesh>();
		//BindMesh(context, fsq_requirements, mesh);


		pipeline_config skybox_render_config;
		skybox_render_config.fill_type = FillType::eFill;
		skybox_render_config.prim_top = PrimitiveTopology::eTriangleList;
		context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VSkyBox]);
		context.BindShader(ShaderStage::Fragment, Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FSkyBox]);
		context.SetCullFace({});
		context.SetDepthTest(false);
		context.SetDepthWrite(false);

		//No idea if this is expensive....if really so I will try shift up to init


		auto camera = gfx_state.camera;

		auto sb_cm = std::get<RscHandle<CubeMap>>(camera.clear_data);

		auto& mesh = camera.CubeMapMesh->as<VulkanMesh>();

		auto mat4block = camera.projection_matrix * mat4{ mat3{ camera.view_matrix } };
		//rs.skyboxRenderer.QueueSkyBox(rs.ubo_manager, {}, *sb_cm, camera.projection_matrix * mat4{ mat3{camera.view_matrix} });

		context.BindUniform("CameraBlock", 0, string_view{ hlp::buffer_data<const char*>(mat4block),hlp::buffer_size(mat4block) });

		context.BindUniform("sb", 0, sb_cm.as<VknCubemap>());
		BindMesh(context, req, mesh);
		//rs.skyboxRenderer.ProcessQueueWithoutRP(cmd_buffer, offset, size);


		//DrawFSQ
		context.DrawIndexed(mesh.IndexCount(), 1, 0, 0, 0);
	}




	ClearCombine::ClearCombine(FrameGraphBuilder& builder, RscHandle<VknRenderTarget>, FrameGraphResource clear_color_buffer, FrameGraphResource scene_color, FrameGraphResource scene_depth, FullRenderData& frd) : PassUtil{ frd }
	{
		auto color_att = CreateGBuffer(builder, "ClearCombine", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor, RscHandle<VknTexture>{render_data.GetGfxState().camera.render_target->GetColorBuffer()});
		//auto depth_att = CreateGBuffer(builder, "DepthCombine", vk::Format::eD16Unorm,    vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth, RscHandle<VknTexture>{rt->GetDepthBuffer()});
		builder.set_output_attachment(color_att, 0,
			AttachmentDescription
			{
					vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp load_op;
					vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp store_op;
					vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
					vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
					vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
					vk::ImageSubresourceRange
					{
						vk::ImageAspectFlagBits::eColor,0,1,0,1
					}
			}
		);
		builder.set_input_attachment(builder.read(clear_color_buffer), 0,
			AttachmentDescription
			{
					vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
					vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp store_op;
					vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
					vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
					vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
					vk::ImageSubresourceRange
					{
						vk::ImageAspectFlagBits::eColor,0,1,0,1
					}
			}
		);
		builder.set_input_attachment(builder.read(scene_color), 1,
			AttachmentDescription
			{
					vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
					vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp store_op;
					vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
					vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
					vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
					vk::ImageSubresourceRange
					{
						vk::ImageAspectFlagBits::eColor,0,1,0,1
					}
			}
		);
		builder.set_input_attachment(builder.read(scene_depth), 2,
			AttachmentDescription
			{
					vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
					vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp store_op;
					vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
					vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
					vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
					vk::ImageSubresourceRange
					{
						vk::ImageAspectFlagBits::eDepth,0,1,0,1
					}
			}
		);
		//builder.set_depth_stencil_attachment(depth_att,
		//	AttachmentDescription
		//	{
		//			vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp load_op;
		//			vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp store_op;
		//			vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
		//			vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
		//			vk::ImageLayout::eDepthStencilAttachmentOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
		//			vk::ImageSubresourceRange
		//			{
		//				vk::ImageAspectFlagBits::eDepth,0,1,0,1
		//			}
		//	}
		//);

	}
	void ClearCombine::Execute(Context_t context)
	{
		context.DebugLabel(RenderTask::LabelLevel::eWhole, "FG: Clear Combine");

		auto& gfx_state = this->render_data.GetGfxState();
		if (!clear_merge)
		{
			auto tmp = Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/clear_merge.frag");
			if (tmp)
				clear_merge = *tmp;
		}
		auto& shd = clear_merge.as<ShaderModule>();
		context.SetViewport(gfx_state.camera.viewport);
		context.SetScissors(gfx_state.camera.viewport);
		if (shd.HasCurrent())
		{

			context.BindShader(ShaderStage::Fragment, clear_merge);
			context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);
			auto& mesh = Mesh::defaults[MeshType::FSQ].as<VulkanMesh>();
			BindMesh(context, this->fsq_requirements, mesh);

			context.DrawIndexed(mesh.IndexCount(), 1, 0, 0, 0);

		}
	}


	std::pair<FrameGraphResource, FrameGraphResource> DeferredRendering::MakePass(FrameGraph& graph, RscHandle<VknRenderTarget> rt, const GraphicsState& gfx_state, RenderStateV2& rs)
	{
		PassUtil::FullRenderData rd{ &gfx_state,&rs };
		auto& gbuffer_pass = graph.addRenderPass<GBufferPass>("GBufferPass", rt, rd);
		auto& cube_clear = graph.addRenderPass<CubeClearPass>("Cube Clear", rd);
		auto& accum_pass = graph.addRenderPass<AccumPass>("Accum pass", gbuffer_pass, rd);
		auto& hdr_pass = graph.addRenderPass<HdrPass>("HDR pass", accum_pass, rt, rd);


		[[maybe_unused]] auto& clear_combine = graph.addRenderPass<ClearCombine>("Clear Combine", rt, cube_clear.render_target, hdr_pass.hdr_rsc, hdr_pass.depth_att, rd);

		return { hdr_pass.hdr_rsc,hdr_pass.depth_att };
	}


	AccumPass::AccumPass(FrameGraphBuilder& builder, GBufferPass& gbuffers) : gbuffer_pass{ gbuffers }
	{
		accum_rsc = CreateGBuffer(builder, "Accum", vk::Format::eR16G16B16A16Sfloat);
		uint32_t index = 0;
		builder.set_output_attachment(accum_rsc, 0, AttachmentDescription
			{
				vk::AttachmentLoadOp::eClear,//vk::AttachmentLoadOp load_op;
				vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp stencil_store_op;
				vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
				vk::ImageSubresourceRange
				{
					vk::ImageAspectFlagBits::eColor,0,1,0,1
				},//vk::ImageSubresourceRange sub_resource_range{};
				vk::ClearColorValue{},//std::optional<vk::ClearValue> clear_value;
				//std::optional<vk::Format> format{};
				//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
				//vk::ComponentMapping mapping{};
			}
		);
		for (auto& gbuffer : gbuffers.gbuffer_rscs)
		{
			builder.set_input_attachment(builder.read(gbuffer), index++, AttachmentDescription
				{
					vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
					vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
					vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
					vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
					vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
					vk::ImageSubresourceRange
					{
						vk::ImageAspectFlagBits::eColor,0,1,0,1
					},//vk::ImageSubresourceRange sub_resource_range{};
					//std::optional<vk::ClearValue> clear_value;
					//std::optional<vk::Format> format{};
					//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
					//vk::ComponentMapping mapping{};
				}
			);
		}
		builder.set_input_attachment(depth_rsc = builder.read(gbuffers.depth_rsc), index++, AttachmentDescription
			{
				vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
				vk::ImageSubresourceRange
				{
					vk::ImageAspectFlagBits::eDepth,0,1,0,1
				},//vk::ImageSubresourceRange sub_resource_range{};
				//std::optional<vk::ClearValue> clear_value;
				//std::optional<vk::Format> format{};
				//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
				//vk::ComponentMapping mapping{};
			}
		);
	}
	void AccumPass::Execute(FrameGraphDetail::Context_t context, BaseDrawSet& draw_set)
	{
		context.attachment_offset = -1;
		context.DebugLabel(RenderTask::LabelLevel::eWhole, "FG: Accum Pass");
		//context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);
		//context.BindShader(ShaderStage::Fragment, Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[deferred_post]);
		//context.BindUniform("irradiance_probe", 0, *RscHandle<VknCubemap>{});
		//context.BindUniform("environment_probe", 0, *RscHandle<VknCubemap>{});
		//context.BindUniform("brdfLUT", 0, *RscHandle<VknTexture>{});
		//for (auto& buffer : gbuffer_rscs)
		{
			size_t i = 0;

			AttachmentBlendConfig blend{};
			blend.blend_enable = true;
			blend.dst_color_blend_factor = BlendFactor::eOne;
			blend.src_color_blend_factor = BlendFactor::eOne;
			blend.color_blend_op = BlendOp::eAdd;
			blend.alpha_blend_op = BlendOp::eMax;
			blend.dst_alpha_blend_factor = BlendFactor::eOne;
			blend.src_alpha_blend_factor = BlendFactor::eOne;
			context.SetBlend(i, blend);
			context.SetClearColor(i, idk::color{ 0,0,0,0 });
			context.SetClearDepthStencil(1.0f);
			++i;
		}
		//context.SetViewport(gfx_state.camera.viewport);
		//context.SetScissors(gfx_state.camera.viewport);

		draw_set.Render(context);
	}
	void HdrPass::Execute(FrameGraphDetail::Context_t context, BaseDrawSet& draw_set)
	{
	}
}