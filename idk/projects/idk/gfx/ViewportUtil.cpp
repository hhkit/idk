#include "stdafx.h"
#include "ViewportUtil.h"
#include <math/rect.h>

namespace idk
{

    //offset,size
    std::pair<ivec2, uivec2> ComputeViewportExtents(const vec2& sz, const rect& vp)
    {
	    std::pair<ivec2, uivec2> result;
	    auto& [offset, size] = result;
	    offset = ivec2{ vec2{ sz }*vp.position };
	    size = uivec2{ vec2{ sz }*(vp.size) };
	    offset = min(max(offset, ivec2{ 0 }), ivec2{ sz });
		size = uivec2{ min(ivec2{ size }+offset, ivec2{ sz }) - offset };
	    size = min(max(size, uivec2{ 1 }), uivec2{ sz }); //prevent the size of the viewport from being too small.
	    return result;
    }

}