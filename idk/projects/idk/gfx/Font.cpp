#include "stdafx.h"
#include <gfx/Font.h>
#include <core/GameObject.h>
#include <common/Transform.h>

namespace idk
{
	void Font::RenderText()
	{

		auto obj_tfm = GetGameObject()->GetComponent<Transform>();
		const char* text_data = text.data();

		vector<FontPoint> coords;
		coords.resize(6*strlen(text_data));
		//array<point,6* strlen(text_data)>

		int n = 0;
		real x = obj_tfm->position.x, y = obj_tfm->position.y, sx = obj_tfm->scale.x, sy = obj_tfm->scale.y;
		real atlas_width = textureAtlas->Size().x, atlas_height = textureAtlas->Size().y;

		auto& c = textureAtlas->c;

		for (const char* p = text_data; *p != '\0'; ++p) {
			float x2 = x + c[*p].bl * sx;
			float y2 = -y - c[*p].bt * sy;
			float w = c[*p].bw * sx;
			float h = c[*p].bh * sy;

			/* Advance the cursor to the start of the next character */
			x += c[*p].ax * sx;
			y += c[*p].ay * sy;

			/* Skip glyphs that have no pixels */
			if (!w || !h)
				continue;

			coords[n++] = { x2,     -y2    , c[*p].tx,                                            0 };
			coords[n++] = { x2 + w, -y2    , c[*p].tx + c[*p].bw / atlas_width,   0 };
			coords[n++] = { x2,     -y2 - h, c[*p].tx,                                          c[*p].bh / atlas_height }; //remember: each glyph occupies a different amount of vertical space
			coords[n++] = { x2 + w, -y2    , c[*p].tx + c[*p].bw / atlas_width,   0 };
			coords[n++] = { x2,     -y2 - h, c[*p].tx,                                          c[*p].bh / atlas_height };
			coords[n++] = { x2 + w, -y2 - h, c[*p].tx + c[*p].bw / atlas_width,                 c[*p].bh / atlas_height };
		}

		fontData.coords = coords;
		fontData.color = colour;
		fontData.fontAtlas = textureAtlas;
		//glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
		//glDrawArrays(GL_TRIANGLES, 0, n);
	}
}