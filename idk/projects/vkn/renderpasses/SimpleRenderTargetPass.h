#pragma once

#include <vkn/DrawSetRenderPass.h>
#include <vkn/FrameGraphBuilder.h>

namespace idk::vkn::renderpasses
{
	class BasicRenderTargetPass : public DrawSetRenderPass
	{
	public:
		FrameGraphResource color_rsc, depth_rsc;
		BasicRenderTargetPass(FrameGraphBuilder& builder, std::optional<color> clear_color, std::optional<float> clear_depth,FrameGraphResource color,FrameGraphResource depth);
		void Execute(Context_t context,BaseDrawSet& draw_set) override;
	private:
	};
}