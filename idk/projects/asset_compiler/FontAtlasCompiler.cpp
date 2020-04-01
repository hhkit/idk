#include <res/ResourceHandle.h>
#include "FontAtlasCompiler.h"
#include <process.h>
#include <windows.h>
#include <filesystem>
#include <iostream>

//Dep
#include <ft2build.h>
#include FT_FREETYPE_H 

#include <gfx/FontAtlas.h>
#include <util/ioutils.h>
#include <serialize/text.inl>

namespace idk
{
	bool CopyCharacter(FT_GlyphSlot glyph, char* output_buffer, size_t offset_x, size_t offset_y, size_t output_pitch)
	{
		auto& bitmap = glyph->bitmap;
		bool copied = false;
		for (size_t src_row = 0; src_row < bitmap.rows; ++src_row)
		{
			size_t src_row_offset = src_row * bitmap.pitch;
			size_t dst_row_offset = (offset_y + (src_row)) * output_pitch;
			size_t dst_col_offset = offset_x;

			memcpy_s(output_buffer + dst_row_offset + dst_col_offset, output_pitch - offset_x, bitmap.buffer + src_row_offset, bitmap.width);
			copied = true;
		}
		return copied;
	}
	opt<AssetBundle> FontAtlasCompiler::LoadAsset(string_view full_path, const MetaBundle& bundle)
	{
		namespace fs = std::filesystem;

		FontAtlasMeta texture_meta{};

		if (auto stringed_meta = bundle.FetchMeta<FontAtlas>())
			if (auto meta = stringed_meta->GetMeta<FontAtlas>())
				texture_meta = *meta;

		auto [t_guid, t_meta] = [&]()
		{
			auto res = bundle.FetchMeta<FontAtlas>();
			return res ? std::make_pair(res->guid, *res->GetMeta<FontAtlas>()) : std::make_pair(Guid::Make(), FontAtlas::Metadata{});
		}();

		auto updated_meta_bundle = bundle;
		updated_meta_bundle.metadatas = { SerializedMeta{ t_guid, "", string{reflect::get_type<FontAtlas>().name()}, serialize_text(t_meta) } };


		// compile the texture
		std::cout << " TEST";

		CompiledFontAtlas t;

		//Font generating
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			fprintf(stderr, "font library not loaded.\n");

			//Free resource when done
			FT_Done_FreeType(ft);
			std::cout << "Font atlas loading generation failed. Crash may happen.\n";
			LOG_TO(LogPool::SYS, "Font atlas loading generation failed. Crash may happen.\n");
			return {};
		}

		ivec2 size{};
		FT_Face face{};

		if (FT_New_Face(ft, full_path.data(), 0, &face))
		{
			fprintf(stderr, "ERROR:: Failed to load font\n");

			//Free resource when done
			FT_Done_Face(face);
			FT_Done_FreeType(ft);
			std::cout << "Font atlas loading generation failed. Crash may happen.\n";
			LOG_TO(LogPool::SYS, "Font atlas loading generation failed. Crash may happen.\n");
			return {};
		}

		if (FT_Set_Pixel_Sizes(face, 0, t_meta.font_size))
		{
			std::cout << "Font atlas loading generation failed. Crash may happen.\n";
			LOG_TO(LogPool::SYS, "Font atlas loading generation failed. Crash may happen.\n");
			return {};
		}

		//Best size generation
		FT_GlyphSlot glyph = face->glyph;
		unsigned w = 0;
		unsigned h = 0;
		unsigned mw = 0;
		unsigned mh = 0;
		static unsigned MAXWIDTH = 1024;


		//First load is to get the best size of the atlas
		for (int i = 32; i < 128; ++i) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				fprintf(stderr, "Loading character %c failed!\n", i);
				continue;
			}
			if (mw + glyph->bitmap.width + 1 >= MAXWIDTH) {
				w = std::max(w, mw);
				h += mh;
				mw = 0;
				mh = 0;
			}

			mw += glyph->bitmap.width + 1;
			mh = std::max(mh, glyph->bitmap.rows);
		}

		w = std::max(mw, w);
		h += mh;
		t.size = uvec2(w, h);
		size = ivec2{ w,h };

		t.ascender = face->ascender / s_cast<float>(face->units_per_EM) * t_meta.font_size;
		t.descender = face->descender / s_cast<float>(face->units_per_EM) * t_meta.font_size;

		int x = 0, y = 0;
		mh = 0;
		size_t spacing = 1;
		auto len = w * h;
		char* raw = (char*)malloc(len);
		std::memset(raw, 0, len);

		t.char_map.resize(128);
		for (int i = 32; i < 128; ++i) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER))
				continue;

			if (x + glyph->bitmap.width + 1 >= MAXWIDTH) {
				y += mh;
				mh = 0;
				x = 0;
			}

			CopyCharacter(glyph, raw, x, y, w);

			t.char_map[i].advance.x = s_cast<float>(glyph->advance.x >> 6);
			t.char_map[i].advance.y = s_cast<float>(glyph->advance.y >> 6);

			t.char_map[i].glyph_size.x = (float)glyph->bitmap.width;
			t.char_map[i].glyph_size.y = (float)glyph->bitmap.rows;

			t.char_map[i].bearing.x = (float)glyph->bitmap_left;
			t.char_map[i].bearing.y = (float)glyph->bitmap_top;

			t.char_map[i].tex_offset.x = x / (float)w;
			t.char_map[i].tex_offset.y = y / (float)h;

			mh = std::max(mh, glyph->bitmap.rows);
			x += int(glyph->bitmap.width + spacing);
		}

		t.pixel_buffer = string_view{raw,len};

		free(raw);

		t.guid = t_guid;
		t.filter_mode = t_meta.filter_mode;
		t.internal_format = t_meta.internal_format;
		t.is_srgb = t_meta.is_srgb;
		t.filter_mode = t_meta.filter_mode;

		return AssetBundle{ updated_meta_bundle, {{ t_guid, std::move(t)} } };;
	}
}