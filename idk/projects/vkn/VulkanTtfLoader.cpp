#include "pch.h"
#include "VulkanTtfLoader.h"
#include <vkn/VknFontAtlas.h>
#include <res/MetaBundle.inl>
#include <vkn/VknFontAtlasLoader.h>
#include <sstream>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceMeta.inl>

//Dep
#include <ft2build.h>
#include FT_FREETYPE_H 

//Resources
#include <gfx/FontAtlas.h>
#include <iostream>

namespace idk::vkn
{
	VulkanView& View();
	TtfLoader::TtfLoader() : allocator{ *View().Device(),View().PDevice() }
	{
		fence = View().Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlagBits::eSignaled });
	}
	vk::Format ToSrgb(vk::Format f);

	bool CopyCharacter(FT_GlyphSlot glyph, char* output_buffer, size_t offset_x, size_t offset_y, size_t output_pitch)
	{
		auto& bitmap = glyph->bitmap;
		bool copied = false;
		for (size_t src_row = 0; src_row < bitmap.rows; ++src_row)
		{
			size_t src_row_offset = src_row * bitmap.pitch;
			size_t dst_row_offset = (offset_y+(src_row)) * output_pitch;
			size_t dst_col_offset = offset_x;

			memcpy_s(output_buffer+dst_row_offset+dst_col_offset, output_pitch - offset_x, bitmap.buffer+src_row_offset, bitmap.width);
			copied = true;
		}
		return copied;
	}
//// 
	ResourceBundle TtfLoader::LoadFile(PathHandle handle, RscHandle<FontAtlas> font_handle, const FontAtlasMeta* tm)
	{

		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			fprintf(stderr, "font library not loaded.\n");

		ivec2 size{};
		FT_Face face{};

		if (FT_New_Face(ft, handle.GetFullPath().data(), 0, &face))
		{
			fprintf(stderr, "ERROR::FREETYPE: Failed to load font\n");

			//Free resource when done
			FT_Done_Face(face);
			FT_Done_FreeType(ft);
		}
		RscHandle<VknFontAtlas> fontAtlas;
		
		FontAtlasMeta new_tm;
		if (!font_handle)
		{
			const auto font = FontAtlas::defaults[FontDefault::SourceSansPro];
			fontAtlas = Core::GetResourceManager().LoaderEmplaceResource<VknFontAtlas>(font.guid);

			if (tm)
				new_tm = *tm;
		}
		else
		{
			fontAtlas = font_handle.guid;
			new_tm = fontAtlas->GetMeta();
		}

		fontAtlas->GetMeta() = new_tm;

		if (FT_Set_Pixel_Sizes(face, 0, new_tm.font_size))
		{
			std::cout << "Font atlas loading generation failed. Crash may happen.\n";
			LOG_TO(LogPool::SYS, "Font atlas loading generation failed. Crash may happen.\n");
			return fontAtlas;
		}
		
		fontAtlas->reload_path = handle;

		FT_GlyphSlot glyph = face->glyph;
		unsigned w = 0;
		unsigned h = 0;
		unsigned mw = 0;
		unsigned mh = 0;
		static unsigned MAXWIDTH = 1024;


		//First load is to get the size of the atlas
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

		/* you might as well save this value as it is needed later on */
		w = std::max(mw, w);
		h += mh;
		fontAtlas->Size(uvec2(w, h));
		size = ivec2{w,h};

		fontAtlas->ascender = face->ascender / s_cast<float>(face->units_per_EM) * new_tm.font_size;
		fontAtlas->descender = face->descender / s_cast<float>(face->units_per_EM) * new_tm.font_size;

		int x = 0, y = 0;
		mh = 0;
		size_t pixel_bytes = 1;
		size_t spacing     = 1;
		auto len = w * h * pixel_bytes;
		char* raw = (char *)malloc(len);
		std::memset(raw, 0, len);
		//unsigned stride = sizeof(unsigned char);
		string_view fml{ raw,len };

		//vector<CharacterCreateInfo> sadList;

		for (int i = 32; i < 128; ++i) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER))
				continue;

			if (x + glyph->bitmap.width + 1 >= MAXWIDTH) {
				y += mh;
				mh = 0;
				x = 0;
			}

			//sadList.emplace_back(CharacterCreateInfo{x,y,glyph->bitmap.width,glyph->bitmap.rows});

			CopyCharacter(glyph, raw, x, y, w* pixel_bytes);

			fontAtlas->c[i].advance.x = s_cast<float>(glyph->advance.x >> 6);
			fontAtlas->c[i].advance.y = s_cast<float>(glyph->advance.y >> 6);

			fontAtlas->c[i].glyph_size.x = (float)glyph->bitmap.width;
			fontAtlas->c[i].glyph_size.y = (float)glyph->bitmap.rows;

			fontAtlas->c[i].bearing.x = (float)glyph->bitmap_left;
			fontAtlas->c[i].bearing.y = (float)glyph->bitmap_top;

			fontAtlas->c[i].tex_offset.x = x / (float)w;
			fontAtlas->c[i].tex_offset.y = y / (float)h;

			mh = std::max(mh, glyph->bitmap.rows);
			//mh = g->bitmap.rows;
			x += int(glyph->bitmap.width + spacing);
		}

		FontAtlasLoader loader;
		FontAtlasOptions def{};
		std::optional<FontAtlasOptions> to{};
		if (tm)
			def = *(to = *tm);

		FACreateInfo tci;
		tci.width = size.x;
		tci.height = size.y;
		tci.layout = vk::ImageLayout::eGeneral;
		tci.mipmap_level = 0;
		tci.sampled(true);
		tci.aspect = vk::ImageAspectFlagBits::eColor;
		tci.internal_format = MapFormat(ToInternalFormat(def.internal_format, def.is_srgb));
		if (def.is_srgb)
			tci.internal_format = ToSrgb(tci.internal_format);
		//TODO detect SRGB and load set format accordingly
		InputFAInfo iti{ r_cast<const char*>(raw),s_cast<size_t>(size.x * size.y *pixel_bytes),def.is_srgb ? vk::Format::eR8Srgb : vk::Format::eR8Uint };
		if (tm)
			to = *tm;
		loader.LoadFontAtlas(*fontAtlas, allocator, *fence, to, tci, iti);
		
		free(raw);

		return fontAtlas;
	}

	ResourceBundle idk::vkn::TtfLoader::LoadFile(PathHandle handle, const MetaBundle& meta)
	{
		auto tex = meta.CreateResource<VknFontAtlas>();
		return LoadFile(handle, RscHandle<FontAtlas>{tex});
	}

}
