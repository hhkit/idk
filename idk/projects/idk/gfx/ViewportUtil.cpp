#include "stdafx.h"
#include "ViewportUtil.h"
#include  <gfx/Viewport.h>
namespace idk
{

//offset,size
std::pair<ivec2, ivec2> ComputeViewportExtents(const vec2& sz, const Viewport& vp)
{
	std::pair<ivec2, ivec2> result;
	auto& [offset, size] = result;
	offset = ivec2{ vec2{ sz }*vp.position };
	size = ivec2{ vec2{ sz }*(vp.size) };
	offset = min(max(offset, ivec2{ 0 }), ivec2{ sz });
	size = min(size + offset, ivec2{ sz }) - offset;
	size = min(max(size, ivec2{ 1 }), ivec2{ sz }); //prevent the size of the viewport from being too small.
	return result;
}

}