#include "stdafx.h"
#include "ViewportUtil.h"
#include <math/rect.h>

namespace idk
{

    //offset,size
    std::pair<ivec2, uvec2> ComputeViewportExtents(const vec2& sz, const rect& vp)
    {
	    std::pair<ivec2, uvec2> result;
	    auto& [offset, size] = result;
	    offset = ivec2{ vec2{ sz }*vp.position };
	    size = uvec2{ vec2{ sz }*(vp.size) };
	    offset = min(max(offset, ivec2{ 0 }), ivec2{ sz });
		size = uvec2{ min(ivec2{ size }+offset, ivec2{ sz }) - offset };
	    size = min(max(size, uvec2{ 1 }), uvec2{ sz }); //prevent the size of the viewport from being too small.
	    return result;
    }

}