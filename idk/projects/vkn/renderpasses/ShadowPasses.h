#pragma once
#include <vkn/RenderInterface.h>
#include <gfx/GraphicsSystem.h>

namespace idk::vkn
{
	struct FrameGraph;
	struct PreRenderData;
}
namespace idk::vkn::renderpasses
{
	//void AddDirectionalShadowPass(FrameGraph& frame_graph, GraphicsSystem::LightRenderRange shadow_range, const PreRenderData& state);
	//void AddSpotlightShadowPass  (FrameGraph& frame_graph, GraphicsSystem::LightRenderRange shadow_range, const PreRenderData& state);
	//void AddPointShadowPass      (FrameGraph& frame_graph, GraphicsSystem::LightRenderRange shadow_range, const PreRenderData& state);
	void AddShadowPass           (FrameGraph& frame_graph, GraphicsSystem::LightRenderRange shadow_range, const PreRenderData& state);
}