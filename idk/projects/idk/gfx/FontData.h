#pragma once

#include <idk.h>
#include <gfx/TextAnchor.h>
#include <gfx/TextAlignment.h>

namespace idk
{
	struct FontPoint
    {
        real x, y, s, t;
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