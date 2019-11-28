#pragma once

#include <idk.h>
#include <gfx/TextAnchor.h>
#include <gfx/TextAlignment.h>

namespace idk
{
	struct FontPoint
    {
        //real x, y, s, t;
		vec4 position;
		
		/*
		FontPoint& operator()(const vec4& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			s = rhs.z;
			t = rhs.w;
			return *this;
		}

		vec4 ConvertToVec4() const
		{
			return vec4{x,y,s,t};
		}
		*/
	};

	struct FontData
    {
		vector<FontPoint> coords;
        real width;
        real height;

        static FontData Generate(string_view text, RscHandle<FontAtlas> font_atlas,
                                 unsigned font_size, real tracking, real line_spacing,
                                 TextAlignment alignment, real wrap_width);
	};
}