#include "stdafx.h"
#include <gfx/Font.h>
#include <core/GameObject.h>
#include <common/Transform.h>

namespace idk
{
	/*Font::Font()
		:Component<Font>{},
		textureAtlas{Core::GetResourceManager().Create<FontAtlas>()}
	{
	}*/
	void Font::RenderText()
	{

		auto obj_tfm = GetGameObject()->GetComponent<Transform>();
		const char* text_data = text.data();

		vector<FontPoint> coords;
		coords.resize(6*strlen(text_data));
		//array<point,6* strlen(text_data)>

		int n = 0;
		real x = obj_tfm->GlobalPosition().x + tracking;
		real y = obj_tfm->GlobalPosition().y + spacing;
		const real sx = obj_tfm->GlobalScale().x;
		const real	sy = obj_tfm->GlobalScale().y;
		const real atlas_width = textureAtlas->Size().x;
		const real atlas_height = textureAtlas->Size().y;

		auto& c = textureAtlas->c;

		for (const char* p = text_data; p && *p != '\0'; ++p) {

			char character = *p;
			real x2 = x + c[character].bearing.x * sx;
			real y2 = y - (c[character].glyph_size.y - c[*p].bearing.y) * sy;
			real w = c[character].glyph_size.x * sx;
			real h = c[character].glyph_size.y * sy;

			/* Advance the cursor to the start of the next character */
			x += c[character].advance.x * sx;
			y += c[character].advance.y * sy;

			/* Skip glyphs that have no pixels */
			if (!w || !h)
				continue;

			coords[n++] = { x2,     y2    , c[character].tx,                                            0 };
			coords[n++] = { x2 + w, y2    , c[character].tx + c[character].glyph_size.x / atlas_width,   0 };
			coords[n++] = { x2,     y2 - h, c[character].tx,                                          c[character].glyph_size.y / atlas_height }; //remember: each glyph occupies a different amount of vertical space
			coords[n++] = { x2 + w, y2    , c[character].tx + c[character].glyph_size.x / atlas_width,   0 };
			coords[n++] = { x2,     y2 - h, c[character].tx,                                          c[character].glyph_size.y / atlas_height };
			coords[n++] = { x2 + w, y2 - h, c[character].tx + c[character].glyph_size.x / atlas_width,       c[character].glyph_size.y / atlas_height };
		}

		fontData.coords = coords;
		fontData.color = colour;
		fontData.fontAtlas = textureAtlas;
		fontData.transform = obj_tfm->GlobalMatrix();
		//glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
		//glDrawArrays(GL_TRIANGLES, 0, n);
	}
}