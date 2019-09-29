#include "pch.h"
#include "OpenGLTexture.h"

namespace idk::ogl
{
	namespace detail
	{
		auto GLUVMode(UVMode uv_mode) -> GLenum
		{
			switch (uv_mode)
			{
			case UVMode::Repeat:       return GL_REPEAT;
			case UVMode::Clamp:        return GL_CLAMP_TO_EDGE;
			case UVMode::MirrorRepeat: return GL_MIRRORED_REPEAT;
			default: return 0;
			}
		}

		auto ToGLColor(ColorFormat f)-> GLenum
		{
			switch (f)
			{
			case ColorFormat::RGB_8:  return GL_RGB8;
			case ColorFormat::RGBA_8: return GL_RGBA8;
			case ColorFormat::RGBF_16: return GL_RGB16F;
			case ColorFormat::RGBF_32: return GL_RGB32F;
			case ColorFormat::RGBAF_16: return GL_RGBA16F;
			case ColorFormat::RGBAF_32: return GL_RGBA32F;
			default: return 0;
			}
		}

		auto ToGLinputChannels(InputChannels i)-> GLint
		{
			switch (i)
			{
			case InputChannels::RED:  return GL_RED;
			case InputChannels::RG:   return GL_RG;
			case InputChannels::RGB:  return GL_RGB;
			case InputChannels::RGBA: return GL_RGBA;
			default: return 0;
			}
		}
	}

	OpenGLTexture::OpenGLTexture()
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		UpdateUV(meta.uv_mode);
		Buffer(nullptr, _size);
	}

	OpenGLTexture::OpenGLTexture(OpenGLTexture&& rhs)
		: Texture{ std::move(rhs) }, _id {rhs._id}
	{
		rhs._id = 0;
	}
	OpenGLTexture& OpenGLTexture::operator=(OpenGLTexture&& rhs)
	{
		Texture::operator=(std::move(rhs));
		std::swap(_id, rhs._id);
		return *this;
	}
	
	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures(1, &_id);
	}
	
	void OpenGLTexture::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, _id);
	}
	
	void OpenGLTexture::BindToUnit(GLuint unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, _id);
	}

	void OpenGLTexture::Buffer(void* data, ivec2 size, InputChannels format)
	{
		_size = size;
		glTexImage2D(GL_TEXTURE_2D, 0, detail::ToGLColor(meta.internal_format), size.x, size.y, 0, detail::ToGLinputChannels(format), GL_UNSIGNED_BYTE, data); // oh no
		glGenerateMipmap(GL_TEXTURE_2D);
		// TODO: fix internal format
		GL_CHECK();
	}


	void OpenGLTexture::Size(ivec2 new_size)
	{
		Texture::Size(new_size);
		Buffer(nullptr, _size);
	}

	void* OpenGLTexture::ID() const
	{
		return r_cast<void*>(_id);
	}

	void OpenGLTexture::OnMetaUpdate(const TextureMeta& tex_meta)
	{
		UpdateUV(tex_meta.uv_mode);
	}
	void OpenGLTexture::UpdateUV(UVMode uv_mode)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, detail::GLUVMode(uv_mode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, detail::GLUVMode(uv_mode));
		GL_CHECK();
	}
}
