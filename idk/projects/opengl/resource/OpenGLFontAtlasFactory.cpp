#include "pch.h"
#include "OpenGLFontAtlasFactory.h"

#include "OpenGLFontAtlas.h"
#include <core/Core.h>

#include <ft2build.h>
#include FT_FREETYPE_H 

#include <iostream>




namespace idk::ogl {
	void OpenGLFontAtlasFactory::GenerateDefaultFontAtlas()
	{
		{	
			/* create SourceSansPro */
			/*
			const auto font = FontAtlas::defaults[FontDefault::SourceSansPro];
			const auto font_handle = Core::GetResourceManager().LoaderEmplaceResource<OpenGLFontAtlas>(font.guid);

			GenerateFontAtlas(PathHandle{ "/engine_data/fonts/SourceSansPro-SemiBold.ttf" }, font_handle);
			*/
		}
	}
	unique_ptr<FontAtlas> OpenGLFontAtlasFactory::GenerateDefaultResource()
	{
		//auto result = *Core::GetResourceManager().Load<OpenGLFontAtlas>("/engine_data/fonts/SourceSansPro-SemiBold.ttf");
		//RscHandle<OpenGLFontAtlas> default_font = Core::GetResourceManager().LoaderEmplaceResource<OpenGLFontAtlas>();
		
		//return GenerateFontAtlas(PathHandle{ "/engine_data/fonts/SourceSansPro-SemiBold.ttf" });
		return {};
	}
	unique_ptr<FontAtlas> OpenGLFontAtlasFactory::Create()
	{
		return GenerateDefaultResource();
	}
}