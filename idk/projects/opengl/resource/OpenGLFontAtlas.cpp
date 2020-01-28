#include "pch.h"
#include "OpenGLFontAtlas.h"
#include <core/Core.inl>
#include <res/ResourceManager.inl>
#include <opengl/resource/OpenGLTextureRenderMeta.h>

namespace idk::ogl {

	OpenGLFontAtlas::OpenGLFontAtlas()
	{
		texture = Core::GetResourceManager().Create<OpenGLTexture>();
	}

	OpenGLFontAtlas::OpenGLFontAtlas(OpenGLFontAtlas&& rhs)
		: FontAtlas{ std::move(rhs) }, 
		texture{ rhs.texture }
	{
		rhs.texture = {};
	}

	OpenGLFontAtlas& OpenGLFontAtlas::operator=(OpenGLFontAtlas&& rhs)
	{
		FontAtlas::operator=(std::move(rhs));
		std::swap(texture, rhs.texture);
		return *this;
	}

	OpenGLFontAtlas::~OpenGLFontAtlas()
	{
		Core::GetResourceManager().Free(texture);
	}

	void OpenGLFontAtlas::Bind()
	{
		texture->Bind();
	}

	void OpenGLFontAtlas::BindToUnit(GLuint texture_unit)
	{
		texture->BindToUnit(texture_unit);
	}

	void OpenGLFontAtlas::Buffer(void* data, size_t buffer_size, uvec2 texture_size, TextureInternalFormat format, GLenum incoming_components, GLenum incoming_type)
	{
		texture->Buffer(data, buffer_size, texture_size, format, incoming_components, incoming_type);
	}


	void OpenGLFontAtlas::Size(uvec2 new_size)
	{
		FontAtlas::Size(new_size);
		texture->Size(new_size);
	}

	void* OpenGLFontAtlas::ID() const
	{
		return texture->ID();
	}

	RscHandle<Texture> OpenGLFontAtlas::Tex() const noexcept
	{
		return RscHandle<Texture>{texture};
	}
}