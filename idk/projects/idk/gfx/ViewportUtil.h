#pragma once
#include <idk.h>
namespace idk
{
	struct rect;
	//offset,size
	std::pair<ivec2, uvec2> ComputeViewportExtents(const vec2& sz, const rect& vp);

}