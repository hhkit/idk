#pragma once
#include <idk.h>
namespace idk
{
	struct Viewport;
	//offset,size
	std::pair<ivec2, ivec2> ComputeViewportExtents(const vec2& sz, const Viewport& vp);

}