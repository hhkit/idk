#include "pch.h"
#include "FrameGraph.h"
#include <vkn/VknRenderTarget.h>

#include <vkn/GraphicsState.h>
#include <vkn/RenderStateV2.h>
#include <vkn/RenderBundle.h>
#include "graph_test.h"

#include <res/ResourceHandle.inl>
#pragma optimize("",off)
namespace idk::vkn::gt
{
	template<typename T>
	string_view to_data(const T& obj)
	{
		return string_view{ reinterpret_cast<const char*>(hlp::buffer_data(obj)), hlp::buffer_size(obj) };
	}
	struct PassUtil : BaseRenderPass
	{
		struct FullRenderData
		{
			const GraphicsState* gfx_state;
			RenderStateV2* rs_state;
			const GraphicsState& GetGfxState()const
			{
				return *gfx_state;
			}
			RenderStateV2& GetRenderState()const
			{
				return *rs_state;
			}
		};
		FullRenderData render_data;

		PassUtil(FullRenderData rd) :render_data{rd} {}

		FrameGraphResourceMutable CreateGBuffer(FrameGraphBuilder& builder, string_view name, vk::Format format, vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits flag = vk::ImageAspectFlagBits::eColor, std::optional<RscHandle<VknTexture>> target = {})
		{
			return builder.write(builder.CreateTexture(TextureDescription
				{
					.name = name,//string_view name);
					.size = uvec2{1920,1080},//ivec2 size);
					.format = format,//vk::Format format);
					.aspect = flag,//vk::ImageAspectFlags aspect);
					//vk::ImageType type = vk::ImageType::e2D);
					//uint32_t layer_count = 1);
					//vk::ImageTiling tiling_format);
					.usage = usage,
					.actual_rsc = target,
				}
			));

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
		}
	};
	struct GBufferPass :PassUtil
	{
		FrameGraphResourceMutable gbuffer_rscs[6];
		FrameGraphResourceMutable depth_rsc;

		GBufferPass(FrameGraphBuilder& builder, RscHandle<VknRenderTarget> rt, FullRenderData& rd) :PassUtil{rd}
		{
			gbuffer_rscs[0] = CreateGBuffer(builder,"AlbedoAmbOcc",vk::Format::eR8G8B8A8Unorm);
			gbuffer_rscs[1] = CreateGBuffer(builder, "Normal", vk::Format::eR8G8B8A8Unorm);
			gbuffer_rscs[2] = CreateGBuffer(builder, "Tangent", vk::Format::eR8G8B8A8Unorm);
			gbuffer_rscs[3] = CreateGBuffer(builder, "eUvMetallicRoughness", vk::Format::eR8G8B8A8Unorm);
			gbuffer_rscs[4] = CreateGBuffer(builder, "ViewPos", vk::Format::eR16G16B16A16Sfloat);
			gbuffer_rscs[5] = CreateGBuffer(builder, "Emissive", vk::Format::eR8G8B8A8Srgb);
			depth_rsc = CreateGBuffer(builder, "GDepth", vk::Format::eD32Sfloat,vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth, RscHandle<VknTexture>{rt->GetDepthBuffer()});
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
		void Execute(FrameGraphDetail::Context_t context) override
		{
			context.SetUboManager(this->render_data.rs_state->ubo_manager);
			auto& gfx_state = this->render_data.GetGfxState();
			context.DebugLabel(RenderTask::LabelLevel::eWhole,"FG: Gbuffer pass");
			std::array description{
					buffer_desc
					{
						buffer_desc::binding_info{ std::nullopt,sizeof(mat4) * 2,VertexRate::eInstance},
						{buffer_desc::attribute_info{AttribFormat::eMat4,4,0,true},
						 buffer_desc::attribute_info{AttribFormat::eMat4,8,sizeof(mat4),true}
						 }
					}
			};
			context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VNormalMesh]);
			context.SetBufferDescriptions(description);
			size_t i = 0;
			for (auto& buffer : gbuffer_rscs)
			{
				context.SetBlend(i);
				context.SetClearColor(i, idk::color{0,0,0,0});
				++i;
			}
			context.SetClearDepthStencil(1.0f);
			context.SetViewport(gfx_state.camera.viewport);
			context.SetScissors(gfx_state.camera.viewport);
			auto mesh_range = index_span{ gfx_state.range.inst_mesh_render_begin,gfx_state.range.inst_mesh_render_end };
			const renderer_attributes* prev_req = {};
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
				context.BindShader(ShaderStage::Fragment,ro.material_instance->material->_shader_program);
				context.BindUniform("CameraBlock", 0, to_data(gfx_state.camera.projection_matrix));
				{
					auto itr = mat_info.uniforms.begin();
					auto end = mat_info.uniforms.end();
					while(itr!=end)
					{
						if (mat_info.IsUniformBlock(itr))
						{
							
							context.BindUniform(itr->first,0,mat_info.GetUniformBlock(itr));
						}
						else if (mat_info.IsImageBlock(itr))
						{
							auto img_block = mat_info.GetImageBlock(itr);
							auto name = string_view{ itr->first };
							name = name.substr(0, name.find_first_of('['));
							for (size_t i = 0; i < img_block.size(); ++i)
							{
								context.BindUniform(name, i,img_block[i].as<VknTexture>() );
							}
						}
						++itr;
					}
				}
				BindMesh(context, req,mesh);
				if (idx_buffer)
				{
					context.BindIndexBuffer(*idx_buffer->buffer(), idx_buffer->offset, mesh.IndexType());
					context.DrawIndexed(mesh.IndexCount(), ro.num_instances, 0, 0, ro.instanced_index);
				}
				else
				{
					context.Draw(mesh.IndexCount(), ro.num_instances, 0, ro.instanced_index);

				}
			}
		}
	};

	struct AccumPass : PassUtil
	{
		FrameGraphResourceMutable accum_rsc;
		FrameGraphResourceReadOnly depth_rsc;
		GBufferPass& gbuffer_pass;

		FragmentShaders deferred_post= FDeferredPost;

		inline const static renderer_attributes fsq_requirements =
		{
			{
				{vtx::Attrib::Position,0},
				{vtx::Attrib::UV,1},
			}
		};
		AccumPass(FrameGraphBuilder& builder, GBufferPass& gbuffers, FullRenderData& rd) :PassUtil{ rd },gbuffer_pass{gbuffers}
		{
			accum_rsc = CreateGBuffer(builder, "Accum", vk::Format::eR16G16B16A16Sfloat);
			uint32_t index = 0;
			builder.set_output_attachment(accum_rsc,0, AttachmentDescription
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
			builder.set_input_attachment(depth_rsc=builder.read(gbuffers.depth_rsc), index++, AttachmentDescription
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
		void Execute(FrameGraphDetail::Context_t context) override
		{
			context.SetUboManager(this->render_data.rs_state->ubo_manager);
			auto& gfx_state = this->render_data.GetGfxState();
			context.DebugLabel(RenderTask::LabelLevel::eWhole, "FG: Accum Pass");
			context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);
			context.BindShader(ShaderStage::Fragment, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[deferred_post]);
			//for (auto& buffer : gbuffer_rscs)
			{
				size_t i = 0;
				
				AttachmentBlendConfig blend{};
				blend.blend_enable           = true;
				blend.dst_color_blend_factor = BlendFactor::eOne;
				blend.src_color_blend_factor = BlendFactor::eOne;
				blend.color_blend_op         = BlendOp::eAdd;
				blend.alpha_blend_op         = BlendOp::eMax;
				blend.dst_alpha_blend_factor = BlendFactor::eOne;
				blend.src_alpha_blend_factor = BlendFactor::eOne;
				context.SetBlend(i,blend);
				context.SetClearColor(i, idk::color{ 0,0,0,0 });
				++i;
			}
			context.SetViewport(gfx_state.camera.viewport);
			context.SetScissors(gfx_state.camera.viewport);

			auto& light_indices = gfx_state.active_lights;
			vector<LightData> lights;
			lights.reserve(8);
			for (size_t i = 0; i < light_indices.size();i+=8)
			{
				for (size_t j = 0; j+i < light_indices.size() && j<8; ++j)
					lights.emplace_back((*gfx_state.shared_gfx_state->lights)[light_indices[i]]);
				auto light_data = PrepareLightBlock(gfx_state.camera, lights);
				context.BindUniform("LightBlock", 0, light_data);
				//Bind all the other uniforms
				//DrawFSQ
				lights.clear();
			}
			BindMesh(context, fsq_requirements,Mesh::defaults[MeshType::FSQ].as<VulkanMesh>());
		}
	};
	
	struct HdrPass : PassUtil
	{
		FrameGraphResourceMutable hdr_rsc;
		AccumPass& accum;
		
		FrameGraphResourceMutable accum_att, depth_att;



		HdrPass(FrameGraphBuilder& builder, AccumPass& accum_,RscHandle<VknRenderTarget> rt, FullRenderData& rd) :PassUtil{ rd } ,accum{accum_}
		{
			hdr_rsc = CreateGBuffer(builder, "HDR", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor, RscHandle<VknTexture>{rt->GetColorBuffer()});
			builder.set_output_attachment(hdr_rsc,0, AttachmentDescription
				{
					vk::AttachmentLoadOp::eClear,//vk::AttachmentLoadOp load_op;
					vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp stencil_store_op;
					vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
					vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
					vk::ImageLayout::eGeneral,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
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
			builder.set_input_attachment(accum_att=builder.read(accum.accum_rsc), 0, AttachmentDescription
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
			builder.set_input_attachment(depth_att=builder.read(accum.depth_rsc), 1, AttachmentDescription
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
		}
		void Execute(FrameGraphDetail::Context_t context) override
		{
		}
	};
	struct CubeClearPass : PassUtil
	{
		FrameGraphResource render_target;
		CubeClearPass(FrameGraphBuilder& builder, FullRenderData& frd) : PassUtil{ frd }
		{
			auto color_att = CreateGBuffer(builder, "ClearColor", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor);
			render_target = color_att;
			std::array<float, 4> clear_color{};
			builder.set_output_attachment(color_att, 0,
				AttachmentDescription
				{
						vk::AttachmentLoadOp::eClear,//vk::AttachmentLoadOp load_op;
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
		void Execute(Context_t)override {}
	};

	struct ClearCombine : PassUtil
	{
		ClearCombine(FrameGraphBuilder& builder, RscHandle<VknRenderTarget> rt, FrameGraphResource clear_color_buffer, FrameGraphResource scene_color, FrameGraphResource scene_depth, FullRenderData& frd) : PassUtil{ frd }
		{
			auto color_att = CreateGBuffer(builder, "ClearCombine", vk::Format::eR8G8B8A8Srgb,vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor, RscHandle<VknTexture>{rt->GetColorBuffer()});
			auto depth_att = CreateGBuffer(builder, "DepthCombine", vk::Format::eD16Unorm,    vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth, RscHandle<VknTexture>{rt->GetDepthBuffer()});
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
			builder.set_depth_stencil_attachment(depth_att,
				AttachmentDescription
				{
						vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp load_op;
						vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp store_op;
						vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
						vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
						vk::ImageLayout::eDepthStencilAttachmentOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
						vk::ImageSubresourceRange
						{
							vk::ImageAspectFlagBits::eDepth,0,1,0,1
						}
				}
			);

		}
		void Execute(Context_t)override {}
	};
	struct DeferredRendering
	{
		//returns color and depth
		std::pair<FrameGraphResource, FrameGraphResource> MakePass(FrameGraph& graph,RscHandle<VknRenderTarget> rt, const GraphicsState& gfx_state, RenderStateV2& rs)
		{
			PassUtil::FullRenderData rd{&gfx_state,&rs};
			auto& gbuffer_pass = graph.addRenderPass<GBufferPass>("GBufferPass", rt     ,rd );
			auto& cube_clear = graph.addRenderPass<CubeClearPass>("Cube Clear", rd);
			auto& accum_pass = graph.addRenderPass<AccumPass>("Accum pass", gbuffer_pass,rd );
			auto& hdr_pass = graph.addRenderPass<HdrPass>("HDR pass", accum_pass,rt     ,rd );


			[[maybe_unused]] auto& clear_combine = graph.addRenderPass<ClearCombine>("Clear Combine", rt, cube_clear.render_target, hdr_pass.hdr_rsc, hdr_pass.depth_att, rd);

			return { hdr_pass.hdr_rsc,hdr_pass.depth_att };
		}
	};

	/*
	GBuffer -> Accum -> HDR -
							  \
	CubeClear        ->       --->  ClearCombine           

	*/

	struct CubeClearRendering
	{
		void MakePass(FrameGraph& graph, RscHandle<VknRenderTarget> rt, FrameGraphResource color,FrameGraphResource depth, const CoreGraphicsState& gfx_state, RenderStateV2& rs)
		{
		}
	};

	void GraphDeferredTest(FrameGraph& fg,const GraphicsState& gfx_state, RenderStateV2& rs)
	{
		fg.Reset();
		DeferredRendering dr;
		CubeClearRendering ccr;
		auto [color,depth] = dr.MakePass(fg, {}, gfx_state, rs);
		//ccr.MakePass(fg, {}, color, depth, gfx_state, rs);
		fg.Compile();
		fg.AllocateResources();
		fg.BuildRenderPasses();
		fg.Execute();
	}

	struct GraphTest::PImpl
	{
		FrameGraph fg;	
	};

	GraphTest::GraphTest() :_pimpl{std::make_unique<PImpl>()}
	{
	}
	void GraphTest::DeferredTest(const GraphicsState& gfx_state, RenderStateV2& rs)
	{
		GraphDeferredTest(_pimpl->fg,gfx_state,rs);
		
		RenderBundle rb{ 
			*rs.cmd_buffer ,
			rs.dpools
		};
		_pimpl->fg.ProcessBatches(rb);
	}
	GraphTest::GraphTest(GraphTest&&) = default;
	GraphTest& GraphTest::operator=(GraphTest&&) = default;
	GraphTest::~GraphTest() = default;
}