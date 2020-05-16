#include "pch.h"
#include "TransparentBlendBindings.h"

namespace idk::vkn::bindings
{

	void TransparentBlendBindings::Bind(RenderInterface& context)
	{
		AttachmentBlendConfig test{};
		test.blend_enable = true;
		test.alpha_blend_op = BlendOp::eAdd;
		test.color_blend_op = BlendOp::eAdd;
		test.src_color_blend_factor = BlendFactor::eSrcAlpha;
		test.dst_color_blend_factor = BlendFactor::eOneMinusSrcAlpha;
		test.src_alpha_blend_factor = BlendFactor::eSrcAlpha;
		test.dst_alpha_blend_factor = BlendFactor::eOneMinusSrcAlpha; //I swear to god idk why this has to be like this, but otherwise, the blend background is just black. eZero didn't work.
		for (auto attachment_idx : attachments)
		{
			context.SetBlend(attachment_idx, test);
		}
	}

}