#pragma once

#include <idk.h>
#include <gfx/TextAnchor.h>
#include <gfx/TextAlignment.h>
#include <res/ResourceHandle.h>
namespace idk
{
	struct FontPoint
    {
        //real x, y, s, t;
		vec4 position{};
		
		std::pair<vec2, vec2> ConvertToPairs() const
		{
			return std::make_pair<vec2,vec2>(position.xy,position.zw);
		}
	};

	struct FontData
    {
		vector<FontPoint> coords{};
		real width{0.f};
		real height{0.f};

		static FontData Generate(string_view text, RscHandle<FontAtlas> font_atlas,
			unsigned font_size, real tracking, real line_spacing,
			TextAlignment alignment, real wrap_width);
	};
}