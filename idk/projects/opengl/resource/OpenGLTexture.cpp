#include "pch.h" 
#include "OpenGLTexture.h"

#include <opengl/resource/OpenGLTextureRenderMeta.h>

namespace idk::ogl
{
	OpenGLTexture::OpenGLTexture()
		: OpenGLTexture{ false }
	{
	}

	OpenGLTexture::OpenGLTexture(const CompiledTexture& compiled_texture)
		: _is_compressed{compiled_texture.is_srgb},
		_mip_level{compiled_texture.generate_mipmaps}
	{
		glGenTextures(1, &_id);
		Bind();
		_size = compiled_texture.size;
		SetUVMode(compiled_texture.mode);
		SetFilteringMode(compiled_texture.filter_mode);

		glCompressedTexImage2D(GL_TEXTURE_2D, _mip_level,
			detail::ogl_GraphicsFormat::ToColor(compiled_texture.internal_format),
			_size.x,
			_size.y,
			0,
			static_cast<GLsizei>(compiled_texture.pixel_buffer.size()),
			compiled_texture.pixel_buffer.data());

		if (_mip_level)
			glGenerateTextureMipmap(_id);
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

	void OpenGLTexture::Buffer(void* data, ivec2 size, InputChannels format, ColorFormat internalFormat, const unsigned& mipmap_size, const float& imgSize)
	{
		_size = size;
		glBindTexture(GL_TEXTURE_2D, _id);
		if (internalFormat == ColorFormat::DEPTH_COMPONENT)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, detail::ogl_GraphicsFormat::ToColor(internalFormat), size.x, size.y, 0, detail::ogl_GraphicsFormat::ToInputChannels(InputChannels::DEPTH_COMPONENT), GL_FLOAT, data); // oh no
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else if(_isCompressedTexture)
		{
			if(size.x && size.y && imgSize)
				glCompressedTexImage2D(GL_TEXTURE_2D, mipmap_size, detail::ogl_GraphicsFormat::ToColor(internalFormat), size.x, size.y, 0, static_cast<GLsizei>(imgSize), data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, detail::ogl_GraphicsFormat::ToColor(internalFormat), size.x, size.y, 0, detail::ogl_GraphicsFormat::ToInputChannels(format), GL_UNSIGNED_BYTE, data); // oh no
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		// TODO: fix internal format
		GL_CHECK();
	}


	ivec2 OpenGLTexture::Size(ivec2 new_size)
	{
		Texture::Size(new_size);
		if(!_is_compressed)
			Buffer(nullptr, _size);
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
