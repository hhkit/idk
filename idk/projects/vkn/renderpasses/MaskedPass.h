#pragma once

#include <vkn/FrameGraphResource.h>

namespace idk::vkn
{
	struct FrameGraph;
	struct GraphicsState;
}

namespace idk::vkn::renderpasses
{
	//Color and Depth
	std::pair<FrameGraphResource,FrameGraphResource> AddTransparentPass(FrameGraph& graph, FrameGraphResource color_att, FrameGraphResource depth_att, const GraphicsState& gfx_state);
}