#pragma once
#include <res/ResourceFactory.h>
#include <idk_opengl/resource/OpenGLTexture.h>

namespace idk::ogl
{
	class OpenGLTextureFactory
		: public ResourceFactory<Texture>
	{
	public:
		unique_ptr<Texture> GenerateDefaultResource() override;
		unique_ptr<Texture> Create() override;
		unique_ptr<Texture> Create(FileHandle filepath) override;
		unique_ptr<Texture> Create(FileHandle filepath, const Texture::Metadata&) override;
	};
}