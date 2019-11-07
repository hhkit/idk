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

		if (textureAtlas)
		{
			vector<FontPoint> coords;
			coords.resize(6 * strlen(text_data));

			int n = 0;
			real x = obj_tfm->GlobalPosition().x, newLine_x = x;
			real y = obj_tfm->GlobalPosition().y;
			const real atlas_width = s_cast<real>(textureAtlas->Size().x);
			const real atlas_height = s_cast<real>(textureAtlas->Size().y);
			const real sx = obj_tfm->GlobalScale().x/48.f;
			const real	sy = obj_tfm->GlobalScale().y/48.f;

			auto& c = textureAtlas->c;

			real newLine_size = c['A'].glyph_size.y;

			for (const char* p = text_data; p && *p != '\0'; ++p) {

				char character = *p;

				real x2 = x + c[character].bearing.x * sx;
				real y2 = -y - c[character].bearing.y * sy;
				real w = c[character].glyph_size.x * sx;
				real h = c[character].glyph_size.y * sy;


				/* Advance the cursor to the start of the next character */
				x += (c[character].advance.x + tracking) * sx;
				y += c[character].advance.y * sy;

				if (character == '\n')
				{
					y += (c[character].advance.y - newLine_size - spacing) * sy;
					x = newLine_x;
				}


				/* Skip glyphs that have no pixels */
				if (!w || !h)
					continue;

				coords[n++] = { x2    , -y2    , c[character].tex_offset.x, c[character].tex_offset.y };

				coords[n++] = { x2    , -y2 - h, c[character].tex_offset.x, c[character].tex_offset.y + c[character].glyph_size.y / atlas_height }; //remember: each glyph occupies a different amount of vertical space

				coords[n++] = { x2 + w, -y2    , c[character].tex_offset.x + c[character].glyph_size.x / atlas_width, c[character].tex_offset.y };

				coords[n++] = { x2 + w, -y2    , c[character].tex_offset.x + c[character].glyph_size.x / atlas_width,  c[character].tex_offset.y };

				coords[n++] = { x2    , -y2 - h, c[character].tex_offset.x, c[character].tex_offset.y + c[character].glyph_size.y / atlas_height };

				coords[n++] = { x2 + w, -y2 - h, c[character].tex_offset.x + c[character].glyph_size.x / atlas_width,  c[character].tex_offset.y + c[character].glyph_size.y / atlas_height };


			}

			fontData.coords = coords;
			fontData.color = colour;
			fontData.fontAtlas = textureAtlas;
			fontData.transform = obj_tfm->GlobalMatrix();
			fontData.n_size = n;
		}
		else
		{
			fontData.coords = {};
			fontData.color = colour;
			fontData.fontAtlas = {};
			fontData.transform = obj_tfm->GlobalMatrix();
			fontData.n_size = 0;
		}
		//glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
		//glDrawArrays(GL_TRIANGLES, 0, n);
	}
	void Font::UpdateFontSize()
	{
		auto m = fontData.fontAtlas->GetMeta();
		m.fontSize = fontSize;
		fontData.fontAtlas->SetMeta(m);

		//fontData.fontAtlas = Core::GetResourceManager().LoaderCreateResource<FontAtlas>(fontData.fontAtlas.guid);
	}
}