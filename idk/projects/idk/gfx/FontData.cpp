#include "stdafx.h"
#include "FontData.h"
#include <res/ResourceMeta.inl>
#include <res/ResourceHandle.inl>
#include <gfx/FontAtlas.h>

namespace idk
{

    FontData FontData::Generate(string_view text, RscHandle<FontAtlas> font_atlas,
                                unsigned font_size, real tracking, real line_spacing,
                                TextAlignment alignment, [[maybe_unused]]real wrap_width)
    {
        if (!font_atlas || text.empty())
            return {};

        FontData data;
        data.coords.resize(6 * text.size());
		//data.width = 0;
		//data.height = 0;

        // screen space
        const real atlas_width = s_cast<real>(font_atlas->Size().x);
        const real atlas_height = s_cast<real>(font_atlas->Size().y);
        const unsigned atlas_font_size = font_atlas->GetMeta().font_size;
        const real s = font_size ? (s_cast<real>(font_size) / atlas_font_size) : 1.0f;

        const auto& c = font_atlas->char_map;
        const real line_height = (font_atlas->ascender - font_atlas->descender) * line_spacing;
        size_t n = 0;
        size_t line_start_n = 0;

        real x = 0;
        real y = -s_cast<real>(font_atlas->ascender) * s;

        for (const char& ch : text)
        {
            auto& character = c[ch];

            real x2 = x + character.bearing.x * s;
            real y2 = -y - character.bearing.y * s;
            real w = character.glyph_size.x * s;
            real h = character.glyph_size.y * s;

            // Advance the cursor to the start of the next character
            x += (character.advance.x + tracking) * s;
            y += character.advance.y * s;

            if (ch == '\n')
            {
                if (alignment == TextAlignment::Center)
                {
                    real d = x * 0.5f;
                    while (line_start_n < n)
                        data.coords[line_start_n++].position.x -= d;
                }
                else if (alignment == TextAlignment::Right)
                {
                    while (line_start_n < n)
                        data.coords[line_start_n++].position.x -= x;
                }

                data.width = x > data.width ? x : data.width;

                y -= line_height * s;
                x = 0;
                continue;
            }

            // Skip glyphs that have no pixels
            if (!w || !h)
                continue;

            //remember: each glyph occupies a different amount of vertical space
            auto x2_w = x2 + w;
            auto y2_h = -y2 - h;
            auto toffset_x = character.tex_offset.x + character.glyph_size.x / atlas_width;
            auto toffset_y = character.tex_offset.y + character.glyph_size.y / atlas_height;

            data.coords[n++].position = vec4{ x2    , -y2    , character.tex_offset.x , character.tex_offset.y };
            data.coords[n++].position = vec4{ x2    , y2_h   , character.tex_offset.x , toffset_y };
            data.coords[n++].position = vec4{ x2_w  , -y2    , toffset_x, character.tex_offset.y };
            data.coords[n++].position = vec4{ x2_w  , -y2    , toffset_x, character.tex_offset.y };
            data.coords[n++].position = vec4{ x2    , y2_h   , character.tex_offset.x , toffset_y };
            data.coords[n++].position = vec4{ x2_w  , y2_h   , toffset_x , toffset_y };
        }

        if(text.back() != '\n')
        {
            if (alignment == TextAlignment::Center)
            {
                real d = x * 0.5f;
                while (line_start_n < n)
                    data.coords[line_start_n++].position.x -= d;
            }
            else if (alignment == TextAlignment::Right)
            {
                while (line_start_n < n)
                    data.coords[line_start_n++].position.x -= x;
            }
            data.width = x > data.width ? x : data.width;
        }

        data.height = -y - font_atlas->descender * s;
        return data;
    }

}