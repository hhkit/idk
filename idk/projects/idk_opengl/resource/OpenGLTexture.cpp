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
			case ColorFormat::sRGB_8:  return GL_RGB8;
			case ColorFormat::sRGBA_8: return GL_RGBA8;
			case ColorFormat::RGBF_16: return GL_RGB16F;
			case ColorFormat::RGBF_32: return GL_RGB32F;
			case ColorFormat::RGBAF_16: return GL_RGBA16F;
			case ColorFormat::RGBAF_32: return GL_RGBA32F;
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

	void OpenGLTexture::Buffer(void* data, ivec2 size, ColorFormat format)
	{
		_size = size;
		glTexImage2D(_id, 0, detail::ToGLColor(meta.internal_format), size.x, size.y, 0, detail::ToGLColor(format), GL_FLOAT, data);
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
	}
}
