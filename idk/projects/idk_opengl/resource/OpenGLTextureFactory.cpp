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
	unique_ptr<Texture> OpenGLTextureFactory::Create(FileHandle filepath)
	{
		return unique_ptr<Texture>();
	}
	unique_ptr<Texture> OpenGLTextureFactory::Create(FileHandle filepath, const Texture::Metadata&)
	{
		return unique_ptr<Texture>();
	}
}