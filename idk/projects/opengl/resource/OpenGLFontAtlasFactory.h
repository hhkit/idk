#pragma once
#include <opengl/resource/OpenGLFontAtlas.h>
#include <res/ResourceFactory.h>
#include <gfx/FontAtlasFactory.h>

namespace idk::ogl
{
	class OpenGLFontAtlasFactory
		: public FontAtlasFactory
	{
	public:
		void GenerateDefaultFontAtlas() override;
		unique_ptr<FontAtlas> GenerateDefaultResource() override;
		unique_ptr<FontAtlas> Create() override;
		};
}