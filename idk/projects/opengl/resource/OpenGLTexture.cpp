#include "pch.h" 
#include "OpenGLTexture.h"

#include <opengl/resource/OpenGLTextureRenderMeta.h>

namespace idk::ogl
{
	OpenGLTexture::OpenGLTexture()
	{
		glGenTextures(1, &_id);
		Bind();
		SetUVMode(UVMode::Clamp);
		SetFilteringMode(FilterMode::Linear);
	}

	OpenGLTexture::OpenGLTexture(const CompiledTexture& compiled_texture)
		: _is_compressed{compiled_texture.is_srgb},
		_mip_level{compiled_texture.generate_mipmaps},
		_internal_format{ ToInternalFormat(compiled_texture.internal_format, true) }
	{
		glGenTextures(1, &_id);
		Bind();
		_size = compiled_texture.size;
		SetUVMode(compiled_texture.mode);
		SetFilteringMode(compiled_texture.filter_mode);

		glCompressedTexImage2D(GL_TEXTURE_2D, _mip_level,
			detail::ogl_GraphicsFormat::ToInternal(_internal_format),
			_size.x,
			_size.y,
			0,
			static_cast<GLsizei>(compiled_texture.pixel_buffer.size()),
			compiled_texture.pixel_buffer.data());

		if (_mip_level)
			glGenerateMipmap(_id);
	}

	OpenGLTexture::OpenGLTexture(OpenGLTexture&& rhs)
		: Texture{ std::move(rhs) }, _id{ rhs._id }, _is_compressed{rhs._is_compressed}, _mip_level{rhs._mip_level}
	{
		rhs._id = 0;
	}

	OpenGLTexture& OpenGLTexture::operator=(OpenGLTexture&& rhs)
	{
		Texture::operator=(std::move(rhs));
		std::swap(_id, rhs._id);
		std::swap(_is_compressed, rhs._is_compressed);
		std::swap(_mip_level, rhs._mip_level);
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
		Bind();
	}

	void OpenGLTexture::Buffer(void* data, size_t buffer_size, ivec2 texture_size, TextureInternalFormat format, GLenum incoming_components, GLenum incoming_type)
	{
		_size = texture_size;
		_internal_format = format;
		auto gl_format = detail::ogl_GraphicsFormat::ToInternal(_internal_format);
		Bind();
		glTexImage2D(GL_TEXTURE_2D, _mip_level, gl_format, _size.x, _size.y, 0, incoming_components, incoming_type, data);
	}

	void OpenGLTexture::Buffer(void* data, size_t buffer_size, ivec2 size, ColorFormat format, bool compressed)
	{
		_size = size;
		Bind();

		_internal_format = ToInternalFormat(format, compressed);
		auto gl_format = detail::ogl_GraphicsFormat::ToInternal(_internal_format);

		if (compressed)
		{
			switch (format)
			{
			case ColorFormat::Alpha_8:
				glTexImage2D(GL_TEXTURE_2D, _mip_level, gl_format, size.x, size.y, 0, GL_R, GL_UNSIGNED_BYTE, data);
				break;
			case ColorFormat::RGB_16bit:
			case ColorFormat::RGB_24bit:
				glTexImage2D(GL_TEXTURE_2D, _mip_level, gl_format, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				break;
			case ColorFormat::RGBA_32bit:
				glTexImage2D(GL_TEXTURE_2D, _mip_level, gl_format, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				break;
			}
		}
		else
		{
			switch (format)
			{
			case ColorFormat::Alpha_8:
				glCompressedTexImage2D(GL_TEXTURE_2D, _mip_level, gl_format, size.x, size.y, 0, buffer_size, data);
				break;
			case ColorFormat::RGB_16bit:
			case ColorFormat::RGB_24bit:
				glCompressedTexImage2D(GL_TEXTURE_2D, _mip_level, gl_format, size.x, size.y, 0, buffer_size, data);
				break;
			case ColorFormat::RGBA_32bit:
				glCompressedTexImage2D(GL_TEXTURE_2D, _mip_level, gl_format, size.x, size.y, 0, buffer_size, data);
				break;
			}
		}
	}


	ivec2 OpenGLTexture::Size(ivec2 new_size)
	{
		Texture::Size(new_size);
		Buffer(nullptr, 0, _size, _internal_format);
		return _size;
	}

	void* OpenGLTexture::ID() const
	{
		return r_cast<void*>(_id);
	}


	void OpenGLTexture::SetUVMode(UVMode uv_mode)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, detail::ogl_GraphicsFormat::ToUVMode(uv_mode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, detail::ogl_GraphicsFormat::ToUVMode(uv_mode));
		GL_CHECK();
	}

	void OpenGLTexture::SetFilteringMode(FilterMode f_mode)
	{
		if (_mip_level)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, detail::ogl_GraphicsFormat::ToFilter(f_mode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, detail::ogl_GraphicsFormat::ToFilter(f_mode));
		}
		GL_CHECK();
	}
}
