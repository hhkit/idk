#pragma once
#include <idk.h>
#include <vkn/FrameGraphResource.h>
#include <vkn/GraphicsState.h>

namespace idk::vkn
{
	struct FrameGraph;
	namespace renderpasses
	{
		std::pair<FrameGraphResource, FrameGraphResource> AddDebugDrawPass(FrameGraph& graph, rect viewport,const GraphicsState& gfx_state,FrameGraphResource color, FrameGraphResource depth);
	}
}