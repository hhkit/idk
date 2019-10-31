#include <pch.h>
#include <opengl/resource/OpenGLFontAtlasLoader.h>
#include <gfx/GraphicsSystem.h>
#include <res/MetaBundle.h>
#include <filesystem>

//Dep
#include <stb/stb_image.h>

//Resources
#include <gfx/FontAtlas.h>
#include <opengl/system/OpenGLGraphicsSystem.h>
#include <opengl/resource/OpenGLFontAtlas.h>

#include <iostream>

namespace idk {
	OpenGLFontAtlasLoader::OpenGLFontAtlasLoader()
	{
		
	}
	ResourceBundle OpenGLFontAtlasLoader::LoadFile(PathHandle path_to_resource, const MetaBundle& metabundle)
	{
		if (FT_Init_FreeType(&ft))
			fprintf(stderr, "font library not loaded.\n");

		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);
		auto meta = metabundle.FetchMeta<FontAtlas>();

		const auto font_handle = meta
			? Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLFontAtlas>(meta->guid)
			: Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLFontAtlas>();

		auto tm = font_handle->GetMeta();
		//auto texture_id = texture_handle->ID();

		tm.font_name = path_to_resource.GetFileName();

		if (meta)
			font_handle->SetMeta(tm);

		ivec2 size{};
		int channels{};

		FT_Face face;
		if (FT_New_Face(ft, path_to_resource.GetFullPath().data(), 0, &face))
		{
			fprintf(stderr, "ERROR::FREETYPE: Failed to load font\n");
			
			//Free resource when done
			FT_Done_Face(face);
			FT_Done_FreeType(ft);

			return font_handle;
		}

		

		FT_Set_Pixel_Sizes(face, 0, tm.fontSize);

		FT_GlyphSlot g = face->glyph;
		unsigned w = 0, h = 0;

		for (int i = 32; i < 128; i++) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				fprintf(stderr, "Loading character %c failed!\n", i);
				continue;
			}

			w += g->bitmap.width;
			h = std::max(h, g->bitmap.rows);
		}

		/* you might as well save this value as it is needed later on */
		int atlas_width = w;
		font_handle->Size(ivec2(w,h));


		//////////////////////////////////Custom Buffer//////////////////////////////////////////
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *s_cast<GLuint*>(font_handle->ID()));
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

		int x = 0;

		for (int i = 32; i < 128; ++i) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER))
				continue;

			glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

			auto p = g->bitmap.buffer;

			font_handle->c[*p].advance.x = g->advance.x >> 6;
			font_handle->c[*p].advance.y = g->advance.y >> 6;

			font_handle->c[*p].glyph_size.x = g->bitmap.width;
			font_handle->c[*p].glyph_size.y = g->bitmap.rows;

			font_handle->c[*p].bearing.x = g->bitmap_left;
			font_handle->c[*p].bearing.y = g->bitmap_top;
							  
			font_handle->c[*p].tx = (float)x / w;

			x += g->bitmap.width;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		GL_CHECK();

		//Free resource when done
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
		/////////////////////////////////////////////////////////////////////////////////////////

		return font_handle;
	}
	
}