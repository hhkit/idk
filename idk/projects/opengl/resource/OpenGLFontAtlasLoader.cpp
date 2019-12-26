#include <pch.h>
#include <opengl/resource/OpenGLFontAtlasLoader.h>
#include <gfx/GraphicsSystem.h>
#include <res/MetaBundle.inl>
#include <filesystem>

//Dep
#include <ft2build.h>
#include FT_FREETYPE_H 

//Resources
#include <gfx/FontAtlas.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <opengl/system/OpenGLGraphicsSystem.h>
#include <res/ResourceMeta.inl>

namespace idk {
	ResourceBundle OpenGLFontAtlasLoader::LoadFile(PathHandle path_to_resource, const MetaBundle& metabundle)
	{
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);
		auto meta = metabundle.FetchMeta<FontAtlas>();

		const auto font_handle = meta
			? Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLFontAtlas>(meta->guid)
			: Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLFontAtlas>();

		
		return ogl::GenerateFontAtlas(path_to_resource, font_handle);
	}

	namespace ogl {
		ResourceBundle GenerateFontAtlas(PathHandle path_to_resource, RscHandle<OpenGLFontAtlas> font_handle)
		{
			FT_Library ft;
			if (FT_Init_FreeType(&ft))
				fprintf(stderr, "font library not loaded.\n");

			ivec2 size{};
			FT_Face face{};

			if (FT_New_Face(ft, path_to_resource.GetFullPath().data(), 0, &face))
			{
				fprintf(stderr, "ERROR::FREETYPE: Failed to load font\n");

				//Free resource when done
				FT_Done_Face(face);
				FT_Done_FreeType(ft);
			}
			if (!font_handle)
			{
				const auto font = FontAtlas::defaults[FontDefault::SourceSansPro];
				font_handle = Core::GetResourceManager().LoaderEmplaceResource<OpenGLFontAtlas>(font.guid);
				
			}

			auto tm = font_handle->GetMeta();

			font_handle->SetMeta(tm);

			if (FT_Set_Pixel_Sizes(face, 0, tm.font_size))
			{
				LOG_TO(LogPool::SYS, "Font atlas loading generation failed. Crash may happen.\n");
				return font_handle;
			}
			font_handle->reload_path = path_to_resource;

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
				//mh = g->bitmap.rows;
			}

			/* you might as well save this value as it is needed later on */
			w = std::max(mw, w);
			h += mh;
			font_handle->Size(ivec2(w, h));

            font_handle->ascender = face->ascender / s_cast<float>(face->units_per_EM) * tm.font_size;
            font_handle->descender = face->descender / s_cast<float>(face->units_per_EM) * tm.font_size;

			//////////////////////////////////Custom Buffer//////////////////////////////////////////
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, s_cast<GLuint>(r_cast<intptr_t>(font_handle->ID())));

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

			GLint current_alignment;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &current_alignment);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			/* Clamping to edges is important to prevent artifacts when scaling */
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			/* Linear filtering usually looks best for text */
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			GL_CHECK();
			int x = 0, y = 0;
			mh = 0;

			for (int i = 32; i < 128; ++i) {
				if (FT_Load_Char(face, i, FT_LOAD_RENDER))
					continue;

				if (x + g->bitmap.width + 1 >= MAXWIDTH) {
					y += mh;
					mh = 0;
					x = 0;
				}

				glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

				font_handle->c[i].advance.x = s_cast<float>(g->advance.x >> 6);
				font_handle->c[i].advance.y = s_cast<float>(g->advance.y >> 6);

				font_handle->c[i].glyph_size.x = (float)g->bitmap.width;
				font_handle->c[i].glyph_size.y = (float)g->bitmap.rows;

				font_handle->c[i].bearing.x = (float)g->bitmap_left;
				font_handle->c[i].bearing.y = (float)g->bitmap_top;

				font_handle->c[i].tex_offset.x = x / (float)w;
				font_handle->c[i].tex_offset.y = y / (float)h;

				mh = std::max(mh, g->bitmap.rows);
				//mh = g->bitmap.rows;
				x += g->bitmap.width + 1;

				//ERROR IN HERE
				GL_CHECK();
			}

			glPixelStorei(GL_UNPACK_ALIGNMENT, current_alignment);

			glBindTexture(GL_TEXTURE_2D, 0);
			GL_CHECK();

			//Free resource when done
			FT_Done_Face(face);
			FT_Done_FreeType(ft);

			return font_handle;
		}

		unique_ptr<FontAtlas> GenerateFontAtlas(PathHandle path_to_resource)
		{
			unique_ptr<OpenGLFontAtlas> font_handle = std::make_unique<OpenGLFontAtlas>();

			FT_Library ft;
			if (FT_Init_FreeType(&ft))
				fprintf(stderr, "font library not loaded.\n");

			ivec2 size{};
			FT_Face face{};

			if (FT_New_Face(ft, path_to_resource.GetFullPath().data(), 0, &face))
			{
				fprintf(stderr, "ERROR::FREETYPE: Failed to load font\n");

				//Free resource when done
				FT_Done_Face(face);
				FT_Done_FreeType(ft);
			}

			auto tm = font_handle->GetMeta();

			font_handle->SetMeta(tm);

			if (FT_Set_Pixel_Sizes(face, 0, tm.font_size))
			{
				LOG_TO(LogPool::SYS, "Font atlas loading generation failed. Crash may happen.\n");
				return font_handle;
			}
			font_handle->reload_path = path_to_resource;

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


			//////////////////////////////////Custom Buffer//////////////////////////////////////////
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, s_cast<GLuint>(r_cast<intptr_t>(font_handle->ID())));

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

			GLint current_alignment;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &current_alignment);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			/* Clamping to edges is important to prevent artifacts when scaling */
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			/* Linear filtering usually looks best for text */
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			GL_CHECK();
			int x = 0, y = 0;
			mh = 0;

			for (int i = 32; i < 128; ++i) {
				if (FT_Load_Char(face, i, FT_LOAD_RENDER))
					continue;

				if (x + g->bitmap.width + 1 >= MAXWIDTH) {
					y += mh;
					mh = 0;
					x = 0;
				}

				glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

				font_handle->c[i].advance.x = s_cast<float>(g->advance.x >> 6);
				font_handle->c[i].advance.y = s_cast<float>(g->advance.y >> 6);
											  
				font_handle->c[i].glyph_size.x = (float)g->bitmap.width;
				font_handle->c[i].glyph_size.y = (float)g->bitmap.rows;

				font_handle->c[i].bearing.x = (float)g->bitmap_left;
				font_handle->c[i].bearing.y = (float)g->bitmap_top;

				font_handle->c[i].tex_offset.x = x / (float)w;
				font_handle->c[i].tex_offset.y = y / (float)h;

				mh = std::max(mh, g->bitmap.rows);
				x += g->bitmap.width + 1;


				//ERROR IN HERE
				GL_CHECK();
			}

			glPixelStorei(GL_UNPACK_ALIGNMENT, current_alignment);

			glBindTexture(GL_TEXTURE_2D, 0);
			GL_CHECK();

			//Free resource when done
			FT_Done_Face(face);
			FT_Done_FreeType(ft);

			return font_handle;
		}
	}
}