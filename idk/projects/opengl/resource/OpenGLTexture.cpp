#include "pch.h" 
#include "OpenGLTexture.h"

#include <opengl/resource/OpenGLTextureRenderMeta.h>

namespace idk::ogl
{
	OpenGLTexture::OpenGLTexture()
		: OpenGLTexture{ false }
	{
	}

	OpenGLTexture::OpenGLTexture(const bool& compressed)
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);

		if (compressed)
		{
			UpdateFilter(FilterMode::Linear,true);
			UpdateUV(UVMode::Clamp);
			_isCompressedTexture = compressed;
		}
		else
		{
			UpdateFilter(FilterMode::Linear);
			UpdateUV(UVMode::Clamp);
			Buffer(nullptr, _size);
		}
	}

	OpenGLTexture::OpenGLTexture(const GLuint& id, const ivec2& size, FilterMode fm, UVMode uv)
	{
		_id = id;
		_size = size;
		UpdateFilter(fm);
		UpdateUV(uv);
	}

	OpenGLTexture::OpenGLTexture(OpenGLTexture&& rhs)
		: Texture{ std::move(rhs) }, _id{ rhs._id }, _isCompressedTexture{rhs._isCompressedTexture}
	{
		rhs._id = 0;
	}
	OpenGLTexture& OpenGLTexture::operator=(OpenGLTexture&& rhs)
	{
		Texture::operator=(std::move(rhs));
		std::swap(_id, rhs._id);
		std::swap(_isCompressedTexture, rhs._isCompressedTexture);
		return *this;
	}
	
	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures(1, &_id);
	}
	
	void OpenGLTexture::Bind()
	{
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _id);
	}
	
	void OpenGLTexture::BindToUnit(GLuint unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, _id);
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

		//glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		// TODO: fix internal format
		GL_CHECK();
	}


	ivec2 OpenGLTexture::Size(ivec2 new_size)
	{
		Texture::Size(new_size);
		if(!_isCompressedTexture)
			Buffer(nullptr, _size);
		return Texture::Size();
	}

	void* OpenGLTexture::ID() const
	{
		return r_cast<void*>(_id);
	}


	void OpenGLTexture::UpdateUV(UVMode uv_mode)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, detail::ogl_GraphicsFormat::ToUVMode(uv_mode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, detail::ogl_GraphicsFormat::ToUVMode(uv_mode));
		GL_CHECK();
	}

	void OpenGLTexture::UpdateFilter(FilterMode f_mode,const bool& isMipMap)
	{
		if(isMipMap)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, detail::ogl_GraphicsFormat::ToFilter(f_mode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, detail::ogl_GraphicsFormat::ToFilter(f_mode));
		GL_CHECK();
	}
}
