#include "pch.h"
#include "BloomPass.h"
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

namespace idk::vkn::renderpasses
{
	using Context_t = PassUtil::Context_t;
	TextureDescription CreateTextureInfo(FrameGraphBuilder& builder, string_view name, vk::Format format, vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits flag = vk::ImageAspectFlagBits::eColor, std::optional<RscHandle<VknTexture>> target = {}, std::optional<uvec2> size = {});
	FrameGraphResourceMutable CreateGBuffer(FrameGraphBuilder& builder, string_view name, vk::Format format, vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits flag = vk::ImageAspectFlagBits::eColor, std::optional<RscHandle<VknTexture>> target = {}, std::optional<uvec2> size = {}, std::optional<WriteOptions> write_opt = {});
	void BindMesh(Context_t context, const renderer_attributes& req, VulkanMesh& mesh);

	BloomPass::BloomPass(FrameGraphBuilder& builder, FrameGraphResource out_color, FrameGraphResource color, FrameGraphResource depth, FrameGraphResource hdr, FrameGraphResource gViewPos, rect viewport) : _viewport{ viewport }
	{
		//bloom_rsc = builder.write(color_tex, WriteOptions{ false });
		//bloom_depth_rsc = CreateGBuffer(builder, "Brightness Depth", vk::Format::eD16Unorm, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth, {}, uvec2{ viewport.size });
		bloom_rsc = builder.write(out_color, WriteOptions{ false });
		//depth_rsc = builder.write(depth);
		//builder.write(combine_.out_color, WriteOptions{false});

		builder.set_output_attachment(bloom_rsc, 0, AttachmentDescription
			{
				vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp load_op;
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
		auto derp1 = builder.read(color);
		auto derp2 = builder.read(depth);
		auto derp3 = builder.read(gViewPos);
		//brightness_read_only = builder.read(hdr, true);
		auto derp4 = builder.read(hdr);

		builder.set_input_attachment(derp1, 1, AttachmentDescription
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
		builder.set_input_attachment(derp2, 2, AttachmentDescription
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
		builder.set_input_attachment(derp3, 3, AttachmentDescription
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

		builder.set_input_attachment(derp4, 4, AttachmentDescription
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

		//_viewport.size = vec2{ rt_size };

		//builder.set_depth_stencil_attachment(depth_rsc, AttachmentDescription
		//	{
		//		vk::AttachmentLoadOp::eClear,//vk::AttachmentLoadOp load_op;
		//		vk::AttachmentStoreOp::eStore,//vk::AttachmentStoreOp stencil_store_op;
		//		vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
		//		vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
		//		vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
		//		vk::ImageSubresourceRange
		//		{
		//			vk::ImageAspectFlagBits::eDepth,0,1,0,1
		//		},//vk::ImageSubresourceRange sub_resource_range{};
		//		vk::ClearDepthStencilValue{},//std::optional<vk::ClearValue> clear_value;
		//		//std::optional<vk::Format> format{};
		//		//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
		//		//vk::ComponentMapping mapping{};
		//	});

	}
	//#pragma optimize("",off)
	void BloomPass::Execute(FrameGraphDetail::Context_t context)
	{
		context.DebugLabel(RenderTask::LabelLevel::eWhole, "FG: Bloom Pass Combine");
		context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);
		if (bloom_shader.guid == Guid{})
		{
			bloom_shader = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FDeferredBloomCombine];//Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/deferred_hdr.frag", false);

		}

		context.BindShader(ShaderStage::Fragment, bloom_shader);
		{
			uint32_t i = 0;

			AttachmentBlendConfig blend{};
			blend.blend_enable = true;
			blend.dst_color_blend_factor = BlendFactor::eOne;
			blend.src_color_blend_factor = BlendFactor::eOne;
			blend.color_blend_op = BlendOp::eAdd;
			blend.alpha_blend_op = BlendOp::eAdd;
			blend.dst_alpha_blend_factor = BlendFactor::eDstAlpha;
			blend.src_alpha_blend_factor = BlendFactor::eSrcAlpha;
			context.SetBlend(i);
			context.SetClearColor(i, idk::color{ 0,0,0,0 });
			context.SetClearDepthStencil(1.0f);
			context.attachment_offset = -1;
			++i;
		}
		//auto hi = context.Resources().Get<VknTextureView>(.id);
		//bright_texture = context.Resources().Get<VknTextureView>(brightness_read_only.id);

		//context.BindUniform("brightness_input", 0, bright_texture, false, vk::ImageLayout::eShaderReadOnlyOptimal);

		context.BindUniform("ColCorrectLut", 0, color_correction_lut);
		PostProcessEffectData toData;
		toData.fogColor = ppe.fogColor;
		toData.fogDensity = ppe.fogDensity;
		toData.threshold = ppe.threshold;
		toData.blurScale = ppe.blurScale;
		toData.blurStrength = ppe.blurStrength;
		toData.useBloom = s_cast<int>(ppe.useBloom);
		toData.useFog = s_cast<int>(ppe.useFog);
		context.BindUniform("PostProcessingBlock", 0, hlp::to_data(toData));

		//context.BindUniform("ViewportBlock", 0, hlp::to_data(_viewport));

		//struct OffsetBlock
		//{
		//	vec2 min;
		//	vec2 extent;
		//};
		//OffsetBlock ob{_viewport.position, _viewport.size };
		//context.BindUniform("ViewportBlock", 0, hlp::to_data(_viewport));
		//_viewport.size = min(vec2(1) - _viewport.position, _viewport.size);
		context.SetViewport(_viewport);
		context.SetScissors(_viewport);


		context.SetCullFace(CullFace::eBack);
		context.SetDepthTest(false);
		//context.SetClearColor(0, idk::color{ 0,0,0,0 });
		//context.SetBlend(0);

		auto& mesh = Mesh::defaults[MeshType::INV_FSQ].as<VulkanMesh>();
		BindMesh(context, fsq_requirements, mesh);

		//DrawFSQ
		context.DrawIndexed(mesh.IndexCount(), 1, 0, 0, 0);
	}

	BloomPassH::BloomPassH(FrameGraphBuilder& builder, FrameGraphResource out_color, FrameGraphResource hdr, rect viewport) : _viewport{ viewport }
	{
		//bloom_rsc = builder.write(color_tex, WriteOptions{ false });
		//bloom_depth_rsc = CreateGBuffer(builder, "Brightness Depth", vk::Format::eD16Unorm, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth, {}, uvec2{ viewport.size });
		bloom_rsc = builder.write(out_color, WriteOptions{ false });

		//builder.write(combine_.out_color, WriteOptions{false});

		builder.set_output_attachment(bloom_rsc, 0, AttachmentDescription
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
		//auto derp1 = builder.read(color);
		brightness_read_only = builder.read(hdr, true);

		//_viewport.size = vec2{ rt_size };

		//builder.set_input_attachment(derp1, 1, AttachmentDescription
		//	{
		//		vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
		//		vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
		//		vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
		//		vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
		//		vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
		//		vk::ImageSubresourceRange
		//		{
		//			vk::ImageAspectFlagBits::eColor,0,1,0,1
		//		},//vk::ImageSubresourceRange sub_resource_range{};
		//		//std::optional<vk::ClearValue> clear_value;
		//		//std::optional<vk::Format> format{};
		//		//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
		//		//vk::ComponentMapping mapping{};
		//	});

	}
	//#pragma optimize("",off)
	void BloomPassH::Execute(FrameGraphDetail::Context_t context)
	{
		context.DebugLabel(RenderTask::LabelLevel::eWhole, "FG: Bloom PassH");
		context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);
		if (bloom_shader.guid == Guid{})
		{
			bloom_shader = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FDeferredBloom];//Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/deferred_hdr.frag", false);

		}

		context.BindShader(ShaderStage::Fragment, bloom_shader);
		{
			uint32_t i = 0;

			AttachmentBlendConfig blend{};
			blend.blend_enable = true;
			blend.dst_color_blend_factor = BlendFactor::eOne;
			blend.src_color_blend_factor = BlendFactor::eOne;
			blend.color_blend_op = BlendOp::eAdd;
			blend.alpha_blend_op = BlendOp::eAdd;
			blend.dst_alpha_blend_factor = BlendFactor::eDstAlpha;
			blend.src_alpha_blend_factor = BlendFactor::eSrcAlpha;
			context.SetBlend(i);
			context.SetClearColor(i, idk::color{ 0,0,0,0 });
			context.SetClearDepthStencil(1.0f);
			context.attachment_offset = -1;
			++i;
		}
		//auto hi = context.Resources().Get<VknTextureView>(.id);
		bright_texture = context.Resources().Get<VknTextureView>(brightness_read_only.id);

		context.BindUniform("brightness_input", 0, bright_texture, false, vk::ImageLayout::eShaderReadOnlyOptimal);

		struct bb {
			int i = 1;
		};

		bb ii;
		context.BindUniform("blurBlock", 0, hlp::to_data(ii));

		PostProcessEffectData toData;
		toData.fogColor = ppe.fogColor;
		toData.fogDensity = ppe.fogDensity;
		toData.threshold = ppe.threshold;
		toData.blurScale = ppe.blurScale;
		toData.blurStrength = ppe.blurStrength;
		toData.useBloom = s_cast<int>(ppe.useBloom);
		toData.useFog = s_cast<int>(ppe.useFog);

		context.BindUniform("PostProcessingBlock", 0, hlp::to_data(toData));
		//_viewport.size = min(vec2(1) - _viewport.position, _viewport.size);
		context.BindUniform("ViewportBlock", 0, hlp::to_data(_viewport));

		context.SetViewport(_viewport);
		context.SetScissors(_viewport);

		context.SetCullFace(CullFace::eBack);
		context.SetDepthTest(false);
		//context.SetClearColor(0, idk::color{ 0,0,0,0 });

		auto& mesh = Mesh::defaults[MeshType::INV_FSQ].as<VulkanMesh>();
		BindMesh(context, fsq_requirements, mesh);

		//DrawFSQ
		context.DrawIndexed(mesh.IndexCount(), 1, 0, 0, 0);
	}

	BloomPassW::BloomPassW(FrameGraphBuilder& builder, FrameGraphResource out_color, FrameGraphResource hdr, rect viewport) : _viewport{ viewport }
	{
		//bloom_rsc = builder.write(color_tex, WriteOptions{ false });
		//bloom_depth_rsc = CreateGBuffer(builder, "Brightness Depth", vk::Format::eD16Unorm, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth, {}, uvec2{ viewport.size });
		bloom_rsc = builder.write(out_color, WriteOptions{ false });

		//builder.write(combine_.out_color, WriteOptions{false});

		builder.set_output_attachment(bloom_rsc, 0, AttachmentDescription
			{
				vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp load_op;
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
		//auto derp1 = builder.read(color);
		brightness_read_only = builder.read(hdr, true);

		//_viewport.size = vec2{ rt_size };

		//builder.set_input_attachment(derp1, 1, AttachmentDescription
		//	{
		//		vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
		//		vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
		//		vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
		//		vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
		//		vk::ImageLayout::eShaderReadOnlyOptimal,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
		//		vk::ImageSubresourceRange
		//		{
		//			vk::ImageAspectFlagBits::eColor,0,1,0,1
		//		},//vk::ImageSubresourceRange sub_resource_range{};
		//		//std::optional<vk::ClearValue> clear_value;
		//		//std::optional<vk::Format> format{};
		//		//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
		//		//vk::ComponentMapping mapping{};
		//	});
		//builder.set_input_attachment(brightness_read_only, 2, AttachmentDescription
		//	{
		//		vk::AttachmentLoadOp::eLoad,//vk::AttachmentLoadOp load_op;
		//		vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
		//		vk::AttachmentLoadOp::eDontCare,//vk::AttachmentLoadOp  stencil_load_op;
		//		vk::AttachmentStoreOp::eDontCare,//vk::AttachmentStoreOp stencil_store_op;
		//		vk::ImageLayout::eGeneral,//vk::ImageLayout layout{vk::ImageLayout::eGeneral}; //layout after RenderPass
		//		vk::ImageSubresourceRange
		//		{
		//			vk::ImageAspectFlagBits::eColor,0,1,0,1
		//		},//vk::ImageSubresourceRange sub_resource_range{};
		//		//std::optional<vk::ClearValue> clear_value;
		//		//std::optional<vk::Format> format{};
		//		//vk::ImageViewType view_type{ vk::ImageViewType::e2D };
		//		//vk::ComponentMapping mapping{};
		//	});
	}
	//#pragma optimize("",off)
	void BloomPassW::Execute(FrameGraphDetail::Context_t context)
	{
		context.DebugLabel(RenderTask::LabelLevel::eWhole, "FG: Bloom PassW");
		context.BindShader(ShaderStage::Vertex, Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);
		if (bloom_shader.guid == Guid{})
		{
			bloom_shader = Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FDeferredBloom];//Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/deferred_hdr.frag", false);

		}

		context.BindShader(ShaderStage::Fragment, bloom_shader);
		{
			uint32_t i = 0;

			AttachmentBlendConfig blend{};
			blend.blend_enable = true;
			blend.dst_color_blend_factor = BlendFactor::eOne;
			blend.src_color_blend_factor = BlendFactor::eOne;
			blend.color_blend_op = BlendOp::eAdd;
			blend.alpha_blend_op = BlendOp::eAdd;
			blend.dst_alpha_blend_factor = BlendFactor::eDstAlpha;
			blend.src_alpha_blend_factor = BlendFactor::eSrcAlpha;
			context.SetBlend(i);
			context.SetClearColor(i, idk::color{ 0,0,0,0 });
			context.SetClearDepthStencil(1.0f);
			context.attachment_offset = -1;
			++i;
		}
		//auto hi = context.Resources().Get<VknTextureView>(.id);
		bright_texture = context.Resources().Get<VknTextureView>(brightness_read_only.id);

		context.BindUniform("brightness_input", 0, bright_texture, false, vk::ImageLayout::eShaderReadOnlyOptimal);

		struct bb {
			int i = 0;
		};

		bb ii;
		context.BindUniform("blurBlock", 0, hlp::to_data(ii));


		PostProcessEffectData toData;
		toData.fogColor = ppe.fogColor;
		toData.fogDensity = ppe.fogDensity;
		toData.threshold = ppe.threshold;
		toData.blurScale = ppe.blurScale;
		toData.blurStrength = ppe.blurStrength;
		toData.useBloom = s_cast<int>(ppe.useBloom);
		toData.useFog = s_cast<int>(ppe.useFog);

		context.BindUniform("PostProcessingBlock", 0, hlp::to_data(toData));
		//context.BindUniform("ViewportBlock", 0, hlp::to_data(_viewport));
		context.BindUniform("ViewportBlock", 0, hlp::to_data(_viewport));

		context.SetViewport(_viewport);
		context.SetScissors(_viewport);

		context.SetCullFace(CullFace::eBack);
		context.SetDepthTest(false);
		//context.SetClearColor(0, idk::color{ 0,0,0,0 });

		auto& mesh = Mesh::defaults[MeshType::INV_FSQ].as<VulkanMesh>();
		BindMesh(context, fsq_requirements, mesh);

		//DrawFSQ
		context.DrawIndexed(mesh.IndexCount(), 1, 0, 0, 0);
	}

}