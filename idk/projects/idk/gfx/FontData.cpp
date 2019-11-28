#include "stdafx.h"
#include "FontData.h"
#include <gfx/FontAtlas.h>

namespace idk
{

    FontData FontData::Generate(string_view text, RscHandle<FontAtlas> font_atlas,
                                unsigned font_size, real tracking, real line_spacing,
                                TextAlignment alignment, real wrap_width)
    {
        if (!font_atlas || text.empty())
            return {};

        FontData data;
        data.coords.resize(6 * text.size());

        // screen space
        const real atlas_width = s_cast<real>(font_atlas->Size().x);
        const real atlas_height = s_cast<real>(font_atlas->Size().y);
        const unsigned atlas_font_size = font_atlas->GetMeta().font_size;
        const real s = font_size ? (s_cast<real>(font_size) / atlas_font_size) : 1.0f;

        const auto& c = font_atlas->c;
        const real line_height = (font_atlas->ascender - font_atlas->descender) * line_spacing;
        size_t n = 0;
        size_t line_start_n = 0;

        real x = 0;
        real y = -s_cast<real>(font_atlas->ascender) * s;

        for (char ch : text)
        {
            real x2 = x + c[ch].bearing.x * s;
            real y2 = -y - c[ch].bearing.y * s;
            real w = c[ch].glyph_size.x * s;
            real h = c[ch].glyph_size.y * s;

            // Advance the cursor to the start of the next character
            x += (c[ch].advance.x + tracking) * s;
            y += c[ch].advance.y * s;

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
            data.coords[n++].position = vec4{ x2    , -y2    , c[ch].tex_offset.x                                   , c[ch].tex_offset.y };
            data.coords[n++].position = vec4{ x2    , -y2 - h, c[ch].tex_offset.x                                   , c[ch].tex_offset.y + c[ch].glyph_size.y / atlas_height };
            data.coords[n++].position = vec4{ x2 + w, -y2    , c[ch].tex_offset.x + c[ch].glyph_size.x / atlas_width, c[ch].tex_offset.y };
            data.coords[n++].position = vec4{ x2 + w, -y2    , c[ch].tex_offset.x + c[ch].glyph_size.x / atlas_width, c[ch].tex_offset.y };
            data.coords[n++].position = vec4{ x2    , -y2 - h, c[ch].tex_offset.x                                   , c[ch].tex_offset.y + c[ch].glyph_size.y / atlas_height };
            data.coords[n++].position = vec4{ x2 + w, -y2 - h, c[ch].tex_offset.x + c[ch].glyph_size.x / atlas_width, c[ch].tex_offset.y + c[ch].glyph_size.y / atlas_height };
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