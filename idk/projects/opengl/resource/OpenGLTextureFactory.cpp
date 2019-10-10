#include "pch.h"
#include "OpenGLTextureFactory.h"

namespace idk::ogl
{
	unique_ptr<Texture> OpenGLTextureFactory::GenerateDefaultResource()
	{
		return std::make_unique<OpenGLTexture>();
	}
	unique_ptr<Texture> OpenGLTextureFactory::Create()
	{
		return std::make_unique<OpenGLTexture>();
	}
}