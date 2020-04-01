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

#include <vkn/DeferredPbrRoBind.h>
#include <vkn/LightBinding.h>
#include <vkn/SkyboxBinding.h>

#include <vkn/renderpasses/BloomPass.h>

namespace idk::vkn::renderpasses
{
	using Context_t = PassUtil::Context_t;

	TextureDescription CreateTextureInfo(FrameGraphBuilder& builder, string_view name, vk::Format format, vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits flag = vk::ImageAspectFlagBits::eColor, std::optional<RscHandle<VknTexture>> target = {}, std::optional<uvec2> size={});
	FrameGraphResourceMutable CreateGBuffer(FrameGraphBuilder& builder, string_view name, vk::Format format, vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits flag = vk::ImageAspectFlagBits::eColor, std::optional<RscHandle<VknTexture>> target = {}, std::optional<uvec2> size = {}, std::optional<WriteOptions> write_opt = {});
	void BindMesh(Context_t context, const renderer_attributes& req, VulkanMesh& mesh);

	TextureDescription CreateTextureInfo([[maybe_unused]] FrameGraphBuilder& builder, string_view name, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlagBits flag, std::optional<RscHandle<VknTexture>> target, std::optional<uvec2> size)
	{
		if (!size)
			size = uvec2{ 1920,1080 };


		return TextureDescription
		{
			.name = name,//string_view name);
			.size = *size,//ivec2 size);
			.format = format,//vk::Format format);
			.aspect = flag,//vk::ImageAspectFlags aspect);
						   //vk::ImageType type = vk::ImageType::e2D);
						   //uint32_t layer_count = 1);
						   //vk::ImageTiling tiling_format);
						   .usage = usage,
						   .actual_rsc = target,
		};

	}

	FrameGraphResourceMutable CreateGBuffer(FrameGraphBuilder& builder, string_view name, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlagBits flag, std::optional<RscHandle<VknTexture>> target, std::optional<uvec2>  size, std::optional<WriteOptions> write_opt)
	{
		WriteOptions opt = {};
		if (write_opt)
			opt = *write_opt;
		return builder.write(builder.CreateTexture(CreateTextureInfo(builder, name, format, usage, flag, target,size)),opt);

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

	GBufferPass::GBufferPass(FrameGraphBuilder& builder, uvec2 size, FrameGraphResource depth) : rt_size{size}
	{
		gbuffer_rscs[0] = CreateGBuffer(builder, "AlbedoAmbOcc", vk::Format::eR8G8B8A8Unorm        ,vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor,{},rt_size);
		gbuffer_rscs[1] = CreateGBuffer(builder, "eUvMetallicRoughness", vk::Format::eR8G8B8A8Unorm,vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor,{},rt_size);
		gbuffer_rscs[2] = CreateGBuffer(builder, "ViewPos", vk::Format::eR16G16B16A16Sfloat        ,vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor,{},rt_size);
		gbuffer_rscs[3] = CreateGBuffer(builder, "Normal", vk::Format::eR8G8B8A8Unorm              ,vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor,{},rt_size);
		//gbuffer_rscs[4] = CreateGBuffer(builder, "Tangent", vk::Format::eR8G8B8A8Unorm             ,vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor,{},rt_size);
		gbuffer_rscs[4] = CreateGBuffer(builder, "Emissive", vk::Format::eB10G11R11UfloatPack32    ,vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor,{},rt_size);
		
		depth_rsc = builder.write(depth, WriteOptions{ false });
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
				vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
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

	AccumPass::AccumPass(FrameGraphBuilder& builder, GBufferPass& gbuffers) : gbuffer_pass{ gbuffers }, rt_size{ gbuffers.rt_size }
	{
		accum_rsc = CreateGBuffer(builder, "Accum", vk::Format::eB10G11R11UfloatPack32, vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor, {}, rt_size);
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
			uint32_t i = 0;

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

	CombinePass::CombinePass(FrameGraphBuilder& builder, [[maybe_unused]] rect viewport, FrameGraphResource in_color_tex, FrameGraphResource in_depth_tex, FrameGraphResource out_color_tex, FrameGraphResource out_depth_tex)
		:_viewport{viewport}
	{
		auto out_color_rsc= builder.write(out_color_tex, WriteOptions{ false });
		auto size = builder.rsc_manager.GetResourceDescription(out_color_tex.id)->actual_rsc->as<VknTexture>().Size();
		auto out_hdr_rsc = CreateGBuffer(builder, "brightness layer", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor, {}, uvec2{ size });
		auto out_depth_rsc= builder.write(out_depth_tex);//CreateGBuffer(builder, "Depth", vk::Format::eD16Unorm, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth,{},accum_def.rt_size);
		out_color = out_color_rsc;
		out_hdr = out_hdr_rsc;
		out_depth = out_depth_rsc;

		builder.set_output_attachment(out_color_rsc, 0, AttachmentDescription
			{
				vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
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
		);;
		builder.set_output_attachment(out_hdr_rsc, 3, AttachmentDescription
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
		);;
		auto in_color = builder.read(in_color_tex);
		auto in_depth = builder.read(in_depth_tex);

		builder.set_input_attachment(in_color, 1, AttachmentDescription
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
		builder.set_input_attachment(in_depth_tex, 2, AttachmentDescription
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
		builder.set_depth_stencil_attachment(out_depth_rsc, AttachmentDescription
			{
				vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
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

	void CombinePass::Execute(FrameGraphDetail::Context_t context)
	{
		context.DebugLabel(RenderTask::LabelLevel::eWhole, name);
		context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);
		if (combine_shader.guid == Guid{})
		{
			combine_shader = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FDeferredCombine];//Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/deferred_hdr.frag", false);

		}


		context.BindShader(ShaderStage::Fragment, combine_shader);

		PostProcessEffectData toData;
		toData.fogColor = ppe.fogColor;
		toData.fogDensity = ppe.fogDensity;
		toData.threshold = ppe.threshold;
		toData.blurScale = ppe.blurScale;
		toData.blurStrength = ppe.blurStrength;
		toData.useBloom = s_cast<int>(ppe.useBloom);
		toData.useFog = s_cast<int>(ppe.useFog);
		context.BindUniform("PostProcessingBlock", 0, hlp::to_data(toData));

		context.SetViewport(_viewport);
		context.SetScissors(_viewport);

		context.SetCullFace({});
		context.SetDepthTest(true);

		//uint32_t i = 0;
		
		context.SetBlend(3);
		context.SetClearColor(3, idk::color{ 0,0,0,0 });
		//context.SetClearColor(i, idk::color{ 0,0,0,0 });
	
		

		auto& mesh = Mesh::defaults[MeshType::FSQ].as<VulkanMesh>();
		BindMesh(context, fsq_requirements, mesh);
		
		context.BindUniform("ColCorrectLut", 0, color_correction_lut);

		//DrawFSQ
		context.DrawIndexed(mesh.IndexCount(), 1, 0, 0, 0);
	}

	CombinePassSpec::CombinePassSpec(FrameGraphBuilder& builder, [[maybe_unused]] rect viewport, FrameGraphResource in_color_tex, FrameGraphResource in_depth_tex, FrameGraphResource out_color_tex, FrameGraphResource out_depth_tex)
		:_viewport{ viewport }
	{
		auto out_color_rsc = builder.write(out_color_tex, WriteOptions{ false });
		//auto out_hdr_rsc = CreateGBuffer(builder, "brightness layer", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor, {}, rt_size);
		auto out_depth_rsc = builder.write(out_depth_tex);//CreateGBuffer(builder, "Depth", vk::Format::eD16Unorm, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth,{},accum_def.rt_size);
		out_color = out_color_rsc;
		//out_hdr = out_hdr_rsc;
		out_depth = out_depth_rsc;

		builder.set_output_attachment(out_color_rsc, 0, AttachmentDescription
			{
				vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
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
		);;
		//builder.set_output_attachment(out_hdr_rsc, 3, AttachmentDescription
		//	{
		//		vk::AttachmentLoadOp::eClear,//vk::AttachmentLoadOp load_op;
		//		vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp stencil_store_op;
		//		vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
		//		vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
		//		vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
		//		vk::ImageSubresourceRange
		//		{
		//			vk::ImageAspectFlagBits::eColor,0,1,0,1
		//		},//vk::ImageSubresourceRange sub_resource_range{};
		//		vk::ClearColorValue{},//std::optional<vk::ClearValue> clear_value;
		//		//std::optional<vk::Format> format{};
		//		//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
		//		//vk::ComponentMapping mapping{};
		//	}
		//);;
		auto in_color = builder.read(in_color_tex);
		auto in_depth = builder.read(in_depth_tex);

		builder.set_input_attachment(in_color, 1, AttachmentDescription
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
		builder.set_input_attachment(in_depth_tex, 2, AttachmentDescription
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
		builder.set_depth_stencil_attachment(out_depth_rsc, AttachmentDescription
			{
				vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
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

	void CombinePassSpec::Execute(FrameGraphDetail::Context_t context)
	{
		context.DebugLabel(RenderTask::LabelLevel::eWhole, name);
		context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);
		if (combine_shader.guid == Guid{})
		{
			combine_shader = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FDeferredCombineSpec];//Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/deferred_hdr.frag", false);

		}

		context.BindShader(ShaderStage::Fragment, combine_shader);
		context.SetViewport(_viewport);
		context.SetScissors(_viewport);

		context.SetCullFace({});
		context.SetDepthTest(true);

		//uint32_t i = 0;

		//context.SetBlend(3);
		context.SetClearColor(0, idk::color{ 0,0,0,0 });
		//context.SetClearColor(i, idk::color{ 0,0,0,0 });



		auto& mesh = Mesh::defaults[MeshType::FSQ].as<VulkanMesh>();
		BindMesh(context, fsq_requirements, mesh);

		context.BindUniform("ColCorrectLut", 0, color_correction_lut);

		//DrawFSQ
		context.DrawIndexed(mesh.IndexCount(), 1, 0, 0, 0);
	}

	HdrPass::HdrPass(FrameGraphBuilder& builder, AccumPass& accum_def_, AccumPass& accum_spec_, rect viewport, FrameGraphResource color_tex, FrameGraphResource depth_tex) : accum_def{ accum_def_ }, accum_spec{ accum_spec_ }, _viewport{viewport}
	{
		hdr_rsc = builder.write(color_tex, WriteOptions{ false });
		hdr_depth_rsc = builder.write(depth_tex);//CreateGBuffer(builder, "Depth", vk::Format::eD16Unorm, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth,{},accum_def.rt_size);
		builder.set_output_attachment(hdr_rsc, 0, AttachmentDescription
			{
				vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
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
		auto derp1 = builder.read(accum_def.accum_rsc);
		auto derp2 = builder.read(accum_def.depth_rsc);
		auto derp3 = builder.read(accum_spec.accum_rsc);
		auto derp4 = builder.read(accum_spec.depth_rsc);
		builder.set_input_attachment(accum_att_def = derp1, 1, AttachmentDescription
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
		builder.set_input_attachment(depth_att_def = derp2, 2, AttachmentDescription
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

		builder.set_input_attachment(accum_att_spec = derp3, 3, AttachmentDescription
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
		builder.set_input_attachment(depth_att_spec = derp4, 4, AttachmentDescription
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
		builder.set_depth_stencil_attachment(hdr_depth_rsc, AttachmentDescription
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
		context.DebugLabel(RenderTask::LabelLevel::eWhole, "FG: HDR Pass");
		context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);
		if (hdr_shader.guid == Guid{})
		{
			hdr_shader = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FDeferredHDR];//Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/deferred_hdr.frag", false);
			
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
		context.SetViewport(_viewport);
		context.SetScissors(_viewport);

		context.SetCullFace({});
		//context.SetDepthTest(false);

		auto& mesh = Mesh::defaults[MeshType::FSQ].as<VulkanMesh>();
		BindMesh(context, fsq_requirements, mesh);

		//DrawFSQ
		context.DrawIndexed(mesh.IndexCount(), 1, 0, 0, 0);
	}




	CubeClearPass::CubeClearPass(FrameGraphBuilder& builder, RscHandle<RenderTarget> rt, bool col_dont_care, std::optional<color>clear_col, std::optional<float> clear_dep) : rt_size{rt->Size()}
	{
		auto color_buffer = RscHandle<VknTexture>{rt->GetColorBuffer()};
		auto depth_buffer = RscHandle<VknTexture>{rt->GetDepthBuffer()};
		std::optional<WriteOptions> col_opt{};
		std::optional<WriteOptions> dep_opt{};
		if (!clear_col&&!col_dont_care)
			col_opt = WriteOptions{ false };
		if (!clear_dep)
			dep_opt = WriteOptions{ false };
		auto color_att = CreateGBuffer(builder, "ClearColor", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment       , vk::ImageAspectFlagBits::eColor,color_buffer,rt_size, col_opt);
		auto depth_att = CreateGBuffer(builder, "ClearDepth", vk::Format::eD32Sfloat   , vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth,depth_buffer,rt_size, dep_opt);
		render_target = color_att;
		depth = depth_att;
		std::array<float, 4> clear_color{};
		float clear_depth = (clear_dep)?*clear_dep:0;
		if (clear_col)
			std::copy(clear_col->begin(), clear_col->end(), clear_color.begin());

		builder.set_output_attachment(color_att, 0,
			AttachmentDescription
			{
					(clear_col) ? vk::AttachmentLoadOp::eClear : col_dont_care? vk::AttachmentLoadOp::eDontCare: vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
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
		builder.set_depth_stencil_attachment(depth_att, 
			AttachmentDescription
			{
					(clear_dep) ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
					vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp store_op;
					vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
					vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
					vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
					vk::ImageSubresourceRange
					{
						vk::ImageAspectFlagBits::eColor,0,1,0,1
					},//vk::ImageSubresourceRange sub_resource_range{};
					vk::ClearDepthStencilValue{clear_depth}//std::optional<vk::ClearValue> clear_value;
					//std::optional<vk::Format> format{};
					//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
					//vk::ComponentMapping mapping{};
			}
		);
	}
	void CubeClearPass::Execute(Context_t context,BaseDrawSet& draw_set)
	{
		context.DebugLabel(RenderTask::LabelLevel::eWhole, "FG: Cube Clear");

		draw_set.Render(context);
		//TODO: Move into a draw_set/draw_logic
	}


	struct clear_info
	{
		std::optional<color> clear_color={};
		std::optional<float> clear_depth={};
		bool ignore_color = false;
	};

	struct ClearInfoVisitor
	{
		clear_info operator()(color col)const
		{
			return clear_info{ col,1.0f };
		}
		clear_info operator()([[maybe_unused]] RscHandle<CubeMap> cube)const
		{
			return clear_info{ {},1.0f,true };
		}
		clear_info operator()(DepthOnly )const
		{
			return clear_info{ {},1.0f };
		}
		clear_info operator()(DontClear )const
		{
			return clear_info{};
		}
	};

	clear_info ExtractClearInfo(const CameraData& camera)
	{
		return std::visit(ClearInfoVisitor{},camera.clear_data);
	}

	UnlitPass::UnlitPass(FrameGraphBuilder& builder, FrameGraphResource color_tex, FrameGraphResource depth_tex)
	{
		color_rsc = builder.write(color_tex,WriteOptions{false});
		depth_rsc = builder.write(depth_tex,WriteOptions{false});

		builder.set_output_attachment(color_rsc, 0, AttachmentDescription
			{
				vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
				vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp stencil_store_op;
				vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::ImageLayout::eGeneral,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
				vk::ImageSubresourceRange
				{
					vk::ImageAspectFlagBits::eColor,0,1,0,1
				},//vk::ImageSubresourceRange sub_resource_range{};
				//std::optional<vk::ClearValue> clear_value;
				//std::optional<vk::Format> format{};
				//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
				//vk::ComponentMapping mapping{};
			});
		builder.set_depth_stencil_attachment(depth_rsc, AttachmentDescription
			{
				vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
				vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp stencil_store_op;
				vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
				vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
				vk::ImageLayout::eGeneral,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
				vk::ImageSubresourceRange
				{
					vk::ImageAspectFlagBits::eDepth,0,1,0,1
				},//vk::ImageSubresourceRange sub_resource_range{};
				//vk::ClearDepthStencilValue{},//std::optional<vk::ClearValue> clear_value;
				//std::optional<vk::Format> format{};
				//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
				//vk::ComponentMapping mapping{};
			});
	}

	void UnlitPass::Execute(Context_t context, BaseDrawSet& draw_set)
	{
		context.DebugLabel(RenderTask::LabelLevel::eWhole, name);
		{
			uint32_t i = 0;

			AttachmentBlendConfig blend{};
			blend.blend_enable = false;
			blend.dst_color_blend_factor = BlendFactor::eOne;
			blend.src_color_blend_factor = BlendFactor::eOne;
			blend.color_blend_op = BlendOp::eAdd;
			blend.alpha_blend_op = BlendOp::eMax;
			blend.dst_alpha_blend_factor = BlendFactor::eOne;
			blend.src_alpha_blend_factor = BlendFactor::eOne;
			context.SetBlend(i, blend);
			//context.SetDepthTest(false);
			++i;
		}
		draw_set.Render(context);
	}

	void DrawSetRenderPass::Execute([[maybe_unused]] Context_t& context)
	{
		LOG_ERROR_TO(LogPool::GFX, "DrawSetRenderPass::Execute(Context_t) should not be executed.");
	}
	using DeferredPbrInstDrawSet = GenericDrawSet<bindings::DeferredPbrRoBind, InstMeshDrawSet>;
	using DeferredPbrAniDrawSet  = GenericDrawSet<bindings::DeferredPbrAniBind, SkinnedMeshDrawSet>;
	using AccumLightDrawSet      = GenericDrawSet<bindings::LightBind, PerLightDrawSet>;
	using AccumAmbientDrawSet    = GenericDrawSet<bindings::AmbientBind, FsqDrawSet>;

	using ClearCubeSet = GenericDrawSet<bindings::SkyboxBindings, FsqDrawSet>;
	using DeferredPbrSet = CombinedMeshDrawSet<DeferredPbrAniDrawSet, DeferredPbrInstDrawSet>;
	using AccumDrawSet = CombinedMeshDrawSet<AccumLightDrawSet, AccumAmbientDrawSet>;
	//#pragma optimize("",off)
	std::pair<FrameGraphResource, FrameGraphResource> DeferredRendering::MakePass(FrameGraph& graph, [[maybe_unused]] RscHandle<VknRenderTarget> rt, const GraphicsState& gfx_state, RenderStateV2& rs)
	{
		using AccumPassSetPair = PassSetPair<AccumPass, AccumDrawSet>;
		PassUtil::FullRenderData rd{ &gfx_state,&rs };
		auto [clr_col, clr_dep,ignore_clear] = ExtractClearInfo(gfx_state.camera);
		//TODO: 
		bindings::SkyboxBindings skybox_binding{};
		skybox_binding.SetCamera(gfx_state.camera);
		auto& cube_clear = graph.addRenderPass<PassSetPair<CubeClearPass, ClearCubeSet>>("Cube Clear", ClearCubeSet{ skybox_binding,FsqDrawSet{MeshType::Box} }, gfx_state.camera.render_target, ignore_clear,clr_col, clr_dep).RenderPass();

		auto& def_depth_copy = graph.addRenderPass <CopyDepthPass>("Copy Default Lit depth", cube_clear.rt_size, cube_clear.depth);
		auto& spec_depth_copy = graph.addRenderPass <CopyDepthPass>("Copy Specular depth", cube_clear.rt_size, cube_clear.depth);

		bindings::StandardVertexBindings::StateInfo state;
		state.SetState(gfx_state);
		bindings::DeferredPbrInfo info{
			.viewport = gfx_state.camera.viewport,
			.blend = BlendMode::Opaque,
			.model = ShadingModel::DefaultLit,
			.material_instances = gfx_state.material_instances(),
			.mat_inst_cache = *gfx_state.shared_gfx_state->mat_inst_cache,
			.vertex_state_info = state,
		};
		auto make_gbuffer_set = [&](const bindings::DeferredPbrInfo& info)
		{ 
			return DeferredPbrSet{
				{
					DeferredPbrAniDrawSet{
							bindings::make_deferred_pbr_ani_bind(info),
							SkinnedMeshDrawSet{span{gfx_state.skinned_mesh_render}}
					},
					DeferredPbrInstDrawSet{bindings::make_deferred_pbr_ro_bind(info),
										   InstMeshDrawSet{
												span{
												  gfx_state.shared_gfx_state->instanced_ros->data() + gfx_state.range.inst_mesh_render_begin,gfx_state.shared_gfx_state->instanced_ros->data() + gfx_state.range.inst_mesh_render_end},
												  gfx_state.shared_gfx_state->inst_mesh_render_buffer.buffer()
											}
						},
				}
			};
		};
		auto gbuffer_set =make_gbuffer_set(info);
		auto& gbuffer_pass_def = graph.addRenderPass<PassSetPair<GBufferPass, DeferredPbrSet>>("GBufferPassDeferred", gbuffer_set, cube_clear.rt_size, def_depth_copy.copied_depth).RenderPass();
		bindings::AmbientBind ambient_bindings;
		bindings::LightBind light_bindings;
		auto& vp_bindings = light_bindings.Get<bindings::CameraViewportBindings>();
		auto& ls_bindings = light_bindings.Get<bindings::LightShadowBinding>();
		auto& accum_fsq_bindings = light_bindings.Get<bindings::DeferredLightFsq>();
		vp_bindings.viewport = gfx_state.camera.viewport;
		ls_bindings.SetState(bindings::LightShadowBinding::State{ gfx_state.active_lights,*gfx_state.lights,gfx_state.shadow_maps_2d,gfx_state.camera.view_matrix,gfx_state.camera.view_matrix.inverse() });
		ambient_bindings.SetCamera(gfx_state.camera, gfx_state.shared_gfx_state->BrdfLookupTable);
		accum_fsq_bindings.SetCamera(gfx_state.camera, gfx_state.shared_gfx_state->BrdfLookupTable);
		accum_fsq_bindings.fragment_shader = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[(info.model == ShadingModel::DefaultLit) ? FDeferredPost : FDeferredPostSpecular];
		auto& accum_pass_def = graph.addRenderPass<AccumPassSetPair>("Accum pass Default", AccumDrawSet{ {AccumLightDrawSet{light_bindings},AccumAmbientDrawSet{} } }, gbuffer_pass_def).RenderPass();
		auto& combine_def_pass = graph.addRenderPass<CombinePass>("Combine DefaultLit pass", gfx_state.camera.viewport, accum_pass_def.accum_rsc, accum_pass_def.depth_rsc, cube_clear.render_target, cube_clear.depth);
		combine_def_pass.color_correction_lut = gfx_state.camera.render_target->ColorGradingLut.as<VknTexture>();

		//Bloom pass stage start

		///Bright color pass - Sample bright colours from light emissive, into a texture, and apply 2-pass gaussian blurring effect on the said texture/////////////////////////
		///Bright colors extracted from Combine default pass//////////
		//Now apply horizontal gaussian pass//////////////
		//auto& hi = cube_clear.rt_size;
		//auto& copy_color_pass   = graph.addRenderPass<CopyColorPass>("Copy Color For BloomH", hi, combine_def_pass.out_hdr, vk::ImageLayout::eShaderReadOnlyOptimal);
		//auto& bloom_passH       = graph.addRenderPass<BloomPassH   >("Bloom passH"          , copy_color_pass.original_color, copy_color_pass.copied_color, gfx_state.camera.viewport);
		//auto& copy_color_pass_1 = graph.addRenderPass<CopyColorPass>("Copy Color For BloomW", hi, bloom_passH.bloom_rsc, vk::ImageLayout::eShaderReadOnlyOptimal);
		///////////Now apply vertical gaussian pass////////////////
		//auto& bloom_pass = graph.addRenderPass<BloomPassW>("Bloom passW", copy_color_pass_1.original_color, copy_color_pass_1.copied_color, gfx_state.camera.viewport);
		//auto& copy_color_pass_2 = graph.addRenderPass<CopyColorPass>("Copy Color For Bloom effect", hi, combine_def_pass.out_color);
		//auto& copy_view_pass_ = graph.addRenderPass<CopyColorPass>("Copy viewpos For fog effect", hi, gbuffer_pass_def.gbuffer_rscs[2]);
		//////////Now additive blend the blurred brightness back onto the default pass texture////////
		//auto& bloom_pass_combine = graph.addRenderPass<BloomPass>("Bloom pass", copy_color_pass_2.original_color, copy_color_pass_2.copied_color, combine_def_pass.out_depth, bloom_pass.bloom_rsc, copy_view_pass_.copied_color, gfx_state.camera.viewport);
		//bloom_pass_combine.color_correction_lut = gfx_state.camera.render_target->ColorGradingLut.as<VknTexture>();
		//
		//bloom_passH.ppe = bloom_pass.ppe = bloom_pass_combine.ppe = combine_def_pass.ppe = gfx_state.camera.ppe;
		//Bloom pass stage end

		auto spec_info = info;
		spec_info.model = ShadingModel::Specular;
		auto& gbuffer_pass_spec = graph.addRenderPass<PassSetPair<GBufferPass, DeferredPbrSet>>("GBufferPassSpecular", make_gbuffer_set(spec_info), cube_clear.rt_size, spec_depth_copy.copied_depth).RenderPass();
		accum_fsq_bindings.fragment_shader = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[(spec_info.model == ShadingModel::DefaultLit) ? FDeferredPost : FDeferredPostSpecular];
		auto& accum_pass_spec = graph.addRenderPass<AccumPassSetPair>("Accum pass Specular", AccumDrawSet{ {AccumLightDrawSet{light_bindings},AccumAmbientDrawSet{} } }, gbuffer_pass_spec).RenderPass();


		auto& combine_spec_pass = graph.addRenderPass<CombinePassSpec>("Combine Spec pass", gfx_state.camera.viewport, accum_pass_spec.accum_rsc, accum_pass_spec.depth_rsc, combine_def_pass.out_color, combine_def_pass.out_depth);
		combine_spec_pass.color_correction_lut = gfx_state.camera.render_target->ColorGradingLut.as<VknTexture>();

		spec_info.model = ShadingModel::Unlit;
		auto& unlit_pass = graph.addRenderPass<PassSetPair<UnlitPass,DeferredPbrSet>>("Unlit Pass", make_gbuffer_set(spec_info),
			combine_spec_pass.out_color,
			//combine_spec_pass.out_color,
			combine_spec_pass.out_depth
			).RenderPass();

		//bloom_pass.bloom_rsc;
		//bloom_pass.bloom_depth_rsc;
		return { unlit_pass.color_rsc,unlit_pass.depth_rsc};
	}


	

	CopyDepthPass::CopyDepthPass(FrameGraphBuilder& builder, uvec2 depth_size, FrameGraphResource depth):size{depth_size}
	{
		auto [copy,original] = builder.copy(depth, CopyOptions{ vk::ImageLayout::eShaderReadOnlyOptimal,
			{
				vk::ImageCopy
				{
					vk::ImageSubresourceLayers
					{
					vk::ImageAspectFlagBits::eDepth,
					0,0,1
					},
					vk::Offset3D{0,0,0},
					vk::ImageSubresourceLayers
					{
					vk::ImageAspectFlagBits::eDepth,
					0,0,1
					},
					vk::Offset3D{0,0,0},
					vk::Extent3D{size.x,size.y,1},
				}
			} });
		builder.NoRenderPass();

		copied_depth = copy;

	}

	void CopyDepthPass::Execute(FrameGraphDetail::Context_t )
	{
	}

	CopyColorPass::CopyColorPass(FrameGraphBuilder& builder, uvec2 color_size, FrameGraphResource color, vk::ImageLayout il) :size{ color_size }, c_pass_name{ builder.Get_region_name() }
	{
		auto [copy,original]=builder.copy(color, CopyOptions{ il,
			{
				vk::ImageCopy
				{
					vk::ImageSubresourceLayers
					{
					vk::ImageAspectFlagBits::eColor,
					0,0,1
					},
					vk::Offset3D{0,0,0},
					vk::ImageSubresourceLayers
					{
					vk::ImageAspectFlagBits::eColor,
					0,0,1
					},
					vk::Offset3D{0,0,0},
					vk::Extent3D{size.x,size.y,1},
				}
			} });
		copied_color = copy;
		original_color = original;
		builder.NoRenderPass();
	}

	void CopyColorPass::Execute(FrameGraphDetail::Context_t context)
	{
		context.DebugLabel(RenderTask::LabelLevel::eWhole, "FG: Copy Pass" + c_pass_name);
	}

}