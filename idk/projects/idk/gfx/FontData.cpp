#include "stdafx.h"
#include "FontData.h"
#include <gfx/FontAtlas.h>

namespace idk
{

    FontData FontData::Generate(string_view text, RscHandle<FontAtlas> font_atlas,
                                unsigned font_size, real tracking, real line_height,
                                TextAlignment alignment, real wrap_width)
    {
        if (!font_atlas)
            return {};

        FontData data;
        data.coords.resize(6 * text.size());

        size_t n = 0;

        // screen space
        real x = 0;
        real y = 0;
        const real atlas_width = s_cast<real>(font_atlas->Size().x);
        const real atlas_height = s_cast<real>(font_atlas->Size().y);
        const unsigned atlas_font_size = font_atlas->GetMeta().font_size;
        const real s = s_cast<real>(font_size) / atlas_font_size;

        const auto& c = font_atlas->c;

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
                y -= line_height * s;
                x = 0;
            }

            // Skip glyphs that have no pixels
            if (!w || !h)
                continue;

            //remember: each glyph occupies a different amount of vertical space
            data.coords[n++] = { x2    , -y2    , c[ch].tex_offset.x                                   , c[ch].tex_offset.y };
            data.coords[n++] = { x2    , -y2 - h, c[ch].tex_offset.x                                   , c[ch].tex_offset.y + c[ch].glyph_size.y / atlas_height };
            data.coords[n++] = { x2 + w, -y2    , c[ch].tex_offset.x + c[ch].glyph_size.x / atlas_width, c[ch].tex_offset.y };
            data.coords[n++] = { x2 + w, -y2    , c[ch].tex_offset.x + c[ch].glyph_size.x / atlas_width, c[ch].tex_offset.y };
            data.coords[n++] = { x2    , -y2 - h, c[ch].tex_offset.x                                   , c[ch].tex_offset.y + c[ch].glyph_size.y / atlas_height };
            data.coords[n++] = { x2 + w, -y2 - h, c[ch].tex_offset.x + c[ch].glyph_size.x / atlas_width, c[ch].tex_offset.y + c[ch].glyph_size.y / atlas_height };
        }

        return data;
    }

}