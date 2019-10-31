#include "pch.h"
#include "../OpenGLFontAtlasFactory.h"

namespace idk::ogl {
	unique_ptr<FontAtlas> OpenGLFontAtlasFactory::GenerateDefaultResource()
	{
		return std::make_unique<OpenGLFontAtlas>();
	}
	unique_ptr<FontAtlas> OpenGLFontAtlasFactory::Create()
	{
		return std::make_unique<OpenGLFontAtlas>();
	}
}