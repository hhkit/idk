#include "pch.h"
#include "FrameGraph.h"
#include <vkn/VknRenderTarget.h>

#include <vkn/GraphicsState.h>
#include <vkn/RenderStateV2.h>


namespace idk::vkn::gt
{
	struct PassUtil : BaseRenderPass
	{
		struct FullRenderData
		{
			const CoreGraphicsState* gfx_state;
			RenderStateV2* rs_state;
			const CoreGraphicsState& GetGfxState()const
			{
				return *gfx_state;
			}
			RenderStateV2& GetRenderState()const
			{
				return *rs_state;
			}
		};
		FullRenderData& render_data;

		PassUtil(FullRenderData& rd) :render_data{rd} {}

		FrameGraphResourceMutable CreateGBuffer(FrameGraphBuilder& builder, string_view name, vk::Format format, vk::ImageAspectFlags flag = vk::ImageAspectFlagBits::eColor, std::optional<RscHandle<VknTexture>> target = {})
		{
			return builder.write(builder.CreateTexture(TextureDescription
				{
					.name = name,//string_view name);
					.size = ivec2{1920,1080},//ivec2 size);
					.format = format,//vk::Format format);
					.aspect = flag,//vk::ImageAspectFlags aspect);
					//vk::ImageType type = vk::ImageType::e2D);
					//uint32_t layer_count = 1);
					//vk::ImageTiling tiling_format);
					.actual_rsc = target
				}
			));

		}

	};
	struct GBufferPass :PassUtil
	{
		FrameGraphResourceMutable gbuffer_rscs[6];
		FrameGraphResourceMutable depth_rsc;

		GBufferPass(FrameGraphBuilder& builder, RscHandle<VknRenderTarget> rt, FullRenderData& rd) :PassUtil{rd}
		{
			gbuffer_rscs[0] = CreateGBuffer(builder,"AlbedoAmbOcc",vk::Format::eR8G8B8A8Unorm);
			gbuffer_rscs[1] = CreateGBuffer(builder, "Normal", vk::Format::eR8G8B8Unorm);
			gbuffer_rscs[2] = CreateGBuffer(builder, "Tangent", vk::Format::eR8G8B8Unorm);
			gbuffer_rscs[3] = CreateGBuffer(builder, "eUvMetallicRoughness", vk::Format::eR8G8B8A8Unorm);
			gbuffer_rscs[4] = CreateGBuffer(builder, "ViewPos", vk::Format::eR16G16B16Sfloat);
			gbuffer_rscs[5] = CreateGBuffer(builder, "Emissive", vk::Format::eR8G8B8Srgb);
			depth_rsc = CreateGBuffer(builder, "GDepth", vk::Format::eD32Sfloat, vk::ImageAspectFlagBits::eDepth, RscHandle<VknTexture>{rt->GetDepthBuffer()});
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
		}
	};

	struct AccumPass : PassUtil
	{
		FrameGraphResourceMutable accum_rsc;
		FrameGraphResourceReadOnly depth_rsc;
		GBufferPass& gbuffer_pass;

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
		}
	};
	
	struct HdrPass : PassUtil
	{
		FrameGraphResourceMutable hdr_rsc;
		AccumPass& accum;
		
		FrameGraphResourceMutable accum_att, depth_att;



		HdrPass(FrameGraphBuilder& builder, AccumPass& accum_,RscHandle<VknRenderTarget> rt, FullRenderData& rd) :PassUtil{ rd } ,accum{accum_}
		{
			hdr_rsc = CreateGBuffer(builder, "HDR", vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor, RscHandle<VknTexture>{rt->GetColorBuffer()});
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
			auto color_att = CreateGBuffer(builder, "ClearColor", vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
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
			auto color_att = CreateGBuffer(builder, "ClearCombine", vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor, RscHandle<VknTexture>{rt->GetColorBuffer()});
			auto depth_att = CreateGBuffer(builder, "DepthCombine", vk::Format::eD16Unorm, vk::ImageAspectFlagBits::eDepth, RscHandle<VknTexture>{rt->GetDepthBuffer()});
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
		std::pair<FrameGraphResource, FrameGraphResource> MakePass(FrameGraph& graph,RscHandle<VknRenderTarget> rt, const CoreGraphicsState& gfx_state, RenderStateV2& rs)
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

	void GraphDeferredTest(const CoreGraphicsState& gfx_state, RenderStateV2& rs)
	{
		std::remove_reference_t<FrameGraph::Context_t> context;
		FrameGraph fg;
		DeferredRendering dr;
		CubeClearRendering ccr;
		auto [color,depth] = dr.MakePass(fg, {}, gfx_state, rs);
		//ccr.MakePass(fg, {}, color, depth, gfx_state, rs);
		fg.Compile();
		fg.AllocateResources();
		fg.BuildRenderPasses();
		fg.Execute(context);
	}
}