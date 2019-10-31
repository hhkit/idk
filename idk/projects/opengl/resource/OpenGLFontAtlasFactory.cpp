#include "pch.h"
#include "OpenGLFontAtlasFactory.h"
//#include "OpenGLFontAtlasLoader.h"

//#include "OpenGLFontAtlas.h"
#include <core/Core.h>


namespace idk::ogl {
	void OpenGLFontAtlasFactory::GenerateDefaultFontAtlas()
	{
		{	/* create SourceSansPro */
			const auto font = FontAtlas::defaults[FontDefault::SourceSansPro];
			const auto font_handle = Core::GetResourceManager().LoaderEmplaceResource<OpenGLFontAtlas>(font.guid);
		}
	}
	unique_ptr<FontAtlas> OpenGLFontAtlasFactory::GenerateDefaultResource()
	{
		//auto result = *Core::GetResourceManager().Load<OpenGLFontAtlas>("/engine_data/fonts/SourceSansPro-SemiBold.ttf");
		const auto font = FontAtlas::defaults[FontDefault::SourceSansPro];
		const auto font_handle = Core::GetResourceManager().LoaderEmplaceResource<OpenGLFontAtlas>(font.guid);

		return std::make_unique<OpenGLFontAtlas>(std::move(*font_handle));
	}
	unique_ptr<FontAtlas> OpenGLFontAtlasFactory::Create()
	{
		const auto font = FontAtlas::defaults[FontDefault::SourceSansPro];
		const auto font_handle = Core::GetResourceManager().LoaderEmplaceResource<OpenGLFontAtlas>(font.guid);

		return std::make_unique<OpenGLFontAtlas>(std::move(*font_handle));
	}
}