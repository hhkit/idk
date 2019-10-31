#pragma once
#include <opengl/resource/OpenGLFontAtlas.h>
#include <res/ResourceFactory.h>

namespace idk::ogl
{
	class OpenGLFontAtlasFactory
		: public ResourceFactory<FontAtlas>
	{
	public:
		unique_ptr<FontAtlas> GenerateDefaultResource() override;
		unique_ptr<FontAtlas> Create() override;
		};
}