#pragma once
#include <res/EasyFactory.h>
#include <res/ResourceFactory.h>
#include <opengl/resource/OpenGLTexture.h>

namespace idk::ogl
{
	class OpenGLTextureFactory
		: public EasyFactory<Texture>
	{
	public:
		unique_ptr<Texture> GenerateDefaultResource() override;
		unique_ptr<Texture> Create() override;
	};
}