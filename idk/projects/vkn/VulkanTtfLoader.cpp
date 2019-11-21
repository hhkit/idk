#include "pch.h"
#include "VulkanTtfLoader.h"
#include <vkn/VknFontAtlas.h>
#include <res/MetaBundle.h>
#include <vkn/VknFontAtlasLoader.h>
#include <sstream>
#include <vkn/VulkanWin32GraphicsSystem.h>

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

	ResourceBundle TtfLoader::LoadFile(PathHandle handle, RscHandle<FontAtlas> font_handle, const FontAtlasMeta* tm)
	{
		//VknTexture& tex = rtex.as<VknTexture>();
		//auto file = handle.Open(FS_PERMISSIONS::READ, true);
		//std::stringstream ss;
		//ss << file.rdbuf();
		//auto data = ss.str();
		//ivec2 size{};
		//int num_channels{};
		//auto tex_data = stbi_load_from_memory(r_cast<const stbi_uc*>(data.data()), s_cast<int>(data.size()), &size.x, &size.y, &num_channels, 4);
		//TextureLoader loader;
		//TextureOptions def{};
		//std::optional<TextureOptions> to{};
		//if (tm)
		//	def = *(to = *tm);
		//TexCreateInfo tci;
		//tci.width = size.x;
		//tci.height = size.y;
		//tci.layout = vk::ImageLayout::eGeneral;
		//tci.mipmap_level = 0;
		//tci.sampled(true);
		//tci.aspect = vk::ImageAspectFlagBits::eColor;
		//tci.internal_format = MapFormat(def.internal_format);
		//if (def.is_srgb)
		//	tci.internal_format = ToSrgb(tci.internal_format);
		////TODO detect SRGB and load set format accordingly
		//InputTexInfo iti{ r_cast<const char*>(tex_data),s_cast<size_t>(size.x * size.y * 4),def.is_srgb ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8G8B8A8Unorm };
		//if (tm)
		//	to = *tm;
		//loader.LoadTexture(tex, allocator, *fence, to, tci, iti);
		//stbi_image_free(tex_data);
		//return rtex;

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
		
		if (!font_handle)
		{
			const auto font = FontAtlas::defaults[FontDefault::SourceSansPro];
			fontAtlas = Core::GetResourceManager().LoaderEmplaceResource<VknFontAtlas>(font.guid);

			if (tm)
			{
				fontAtlas->SetMeta(*tm);
				//tm->font_name = handle.GetFileName();

				if (FT_Set_Pixel_Sizes(face, 0, tm->fontSize))
				{
					std::cout << "Font atlas loading generation failed. Crash may happen.\n";
					LOG_TO(LogPool::SYS, "Font atlas loading generation failed. Crash may happen.\n");
					return fontAtlas;
				}
			}
		}
		else
		{
			FontAtlasMeta new_tm = font_handle->GetMeta();

			new_tm.font_name = handle.GetFileName();
			fontAtlas->SetMeta(new_tm);

			if (FT_Set_Pixel_Sizes(face, 0, new_tm.fontSize))
			{
				std::cout << "Font atlas loading generation failed. Crash may happen.\n";
				LOG_TO(LogPool::SYS, "Font atlas loading generation failed. Crash may happen.\n");
				return fontAtlas;
			}
		}
		
		fontAtlas->reload_path = handle;

		FT_GlyphSlot g = face->glyph;
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
			if (mw + g->bitmap.width + 1 >= MAXWIDTH) {
				w = std::max(w, mw);
				h += mh;
				mw = 0;
				mh = 0;
			}

			mw += g->bitmap.width + 1;
			mh = std::max(mh, g->bitmap.rows);
		}

		/* you might as well save this value as it is needed later on */
		w = std::max(mw, w);
		h += mh;
		font_handle->Size(ivec2(w, h));
		size = ivec2{w,h};

		int x = 0, y = 0;
		mh = 0;

		char* raw = (char *)malloc(w*h*sizeof(unsigned));
		unsigned stride = sizeof(unsigned);

		vector<CharacterCreateInfo> sadList;

		for (int i = 32; i < 128; ++i) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER))
				continue;

			if (x + g->bitmap.width + 1 >= MAXWIDTH) {
				y += mh;
				mh = 0;
				x = 0;
			}

			//glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
			for(unsigned row = (unsigned)y; row < g->bitmap.rows; ++row)
				memcpy_s((raw + (row*stride*w) + (x*stride)), g->bitmap.width, g->bitmap.buffer, g->bitmap.width);


			fontAtlas->c[i].advance.x = s_cast<float>(g->advance.x >> 6);
			fontAtlas->c[i].advance.y = s_cast<float>(g->advance.y >> 6);

			fontAtlas->c[i].glyph_size.x = (float)g->bitmap.width;
			fontAtlas->c[i].glyph_size.y = (float)g->bitmap.rows;

			fontAtlas->c[i].bearing.x = (float)g->bitmap_left;
			fontAtlas->c[i].bearing.y = (float)g->bitmap_top;

			fontAtlas->c[i].tex_offset.x = x / (float)w;
			fontAtlas->c[i].tex_offset.y = y / (float)h;

			mh = std::max(mh, g->bitmap.rows);
			//mh = g->bitmap.rows;
			x += g->bitmap.width + 1;
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
		tci.internal_format = MapFormat(def.internal_format);
		if (def.is_srgb)
			tci.internal_format = ToSrgb(tci.internal_format);
		//TODO detect SRGB and load set format accordingly
		InputFAInfo iti{ r_cast<const char*>(raw),s_cast<size_t>(size.x * size.y * 4),def.is_srgb ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8G8B8A8Unorm };
		if (tm)
			to = *tm;
		loader.LoadFontAtlas(*fontAtlas, allocator, *fence, to, tci, iti);
		
		free(raw);

		return fontAtlas;
	}

	ResourceBundle idk::vkn::TtfLoader::LoadFile(PathHandle handle, const MetaBundle& meta)
	{
		auto m = meta.FetchMeta<FontAtlas>();
		auto tex = m ? Core::GetResourceManager().LoaderEmplaceResource<VknFontAtlas>(m->guid)
			: Core::GetResourceManager().LoaderEmplaceResource<VknFontAtlas>();

		return LoadFile(handle, RscHandle<FontAtlas>{tex});
	}

}
