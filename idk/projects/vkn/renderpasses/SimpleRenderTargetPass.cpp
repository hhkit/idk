#include "pch.h"
#include "SimpleRenderTargetPass.h"
namespace idk::vkn::renderpasses
{

	BasicRenderTargetPass::BasicRenderTargetPass(FrameGraphBuilder& builder, std::optional<color> clear_color, std::optional<float> clear_depth, FrameGraphResource color, FrameGraphResource depth)
	{
		FrameGraphResourceMutable 
		color_orsc = builder.write(color),
		depth_orsc = builder.write(depth);
		color_rsc = color_orsc;
		depth_rsc = depth_orsc;
		vk::ClearDepthStencilValue dep = {};
		vk::ClearColorValue col = {};
		if (clear_color)
		{
			col.setFloat32(reinterpret_cast<std::array<float,4>&>(*clear_color));
		}
		if (clear_depth)
		{
			dep.depth = *clear_depth;
		}

		builder.set_output_attachment(color_rsc, 0,
			AttachmentDescription::make(
				(clear_color) ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad,
				vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare,
				vk::AttachmentStoreOp::eDontCare,
				vk::ImageSubresourceRange
				{
					vk::ImageAspectFlagBits::eColor,
					0,1,0,1
				},
				col, {}, {}
			)
		);
		builder.set_depth_stencil_attachment(depth_rsc,
				AttachmentDescription::make(
				(clear_color) ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad,
					vk::AttachmentStoreOp::eStore,
					vk::AttachmentLoadOp::eDontCare,
					vk::AttachmentStoreOp::eDontCare,
					vk::ImageSubresourceRange
					{
						vk::ImageAspectFlagBits::eDepth,
						0,1,0,1
					},
					dep, {}, {}
			)
		);

	}

	void BasicRenderTargetPass::Execute(Context_t context, BaseDrawSet& draw_set)
	{
		draw_set.Render(context);
	}

}