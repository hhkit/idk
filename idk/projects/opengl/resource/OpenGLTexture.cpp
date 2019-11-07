#include "pch.h" 
#include "OpenGLTexture.h"
#ifndef DDSDATA_H
#define DDSDATA_H

#endif

namespace idk::ogl
{
	namespace detail
	{
		auto GLFilter(FilterMode filter_mode) -> GLenum
		{
			switch (filter_mode)
			{
			case FilterMode::Linear:       return GL_LINEAR;
			case FilterMode::Nearest:        return GL_NEAREST;
			default: return 0;
			}
		}

		auto GLUVMode(UVMode uv_mode) -> GLenum
		{
			switch (uv_mode)
			{
			case UVMode::Repeat:       return GL_REPEAT;
			case UVMode::Clamp:        return GL_CLAMP_TO_EDGE;
			case UVMode::MirrorRepeat: return GL_MIRRORED_REPEAT;
			case UVMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
			default: return 0;
			}
		}
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
		auto ToGLColor(ColorFormat f)-> GLenum
		{
			switch (f)
			{
			case ColorFormat::RUI_32: return GL_R32UI;
			case ColorFormat::RG_8: return GL_RG;
			case ColorFormat::RGF_16: return GL_RG16F;
			case ColorFormat::RGB_8:  return GL_RGB8;
			case ColorFormat::RGBA_8: return GL_RGBA8;
			case ColorFormat::RGBF_16: return GL_RGB16F;
			case ColorFormat::RGBF_32: return GL_RGB32F;
			case ColorFormat::RGBAF_16: return GL_RGBA16F;
			case ColorFormat::RGBAF_32: return GL_RGBA32F;
			case ColorFormat::SRGB: return GL_SRGB;
			case ColorFormat::DEPTH_COMPONENT: return GL_DEPTH_COMPONENT;
			case ColorFormat::DXT1: return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			case ColorFormat::DXT3: return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			case ColorFormat::DXT5: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
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
			case InputChannels::DEPTH_COMPONENT: return GL_DEPTH_COMPONENT;
			default: return 0;
			}
		}
	}
	OpenGLTexture::OpenGLTexture()
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);
		
		UpdateFilter(meta.filter_mode);
		UpdateUV(meta.uv_mode);
		Buffer(nullptr, _size);
	}

	OpenGLTexture::OpenGLTexture(const bool& compressed)
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);

		if (compressed)
		{
			UpdateFilter(meta.filter_mode,true);
			UpdateUV(meta.uv_mode);
			_isCompressedTexture = compressed;
		}
		else
		{
			UpdateFilter(meta.filter_mode);
			UpdateUV(meta.uv_mode);
			Buffer(nullptr, _size);
		}
	}

	OpenGLTexture::OpenGLTexture(const GLuint& id, const ivec2& size)
	{
		_id = id;
		_size = size;
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
			glTexImage2D(GL_TEXTURE_2D, 0, detail::ToGLColor(internalFormat), size.x, size.y, 0, detail::ToGLinputChannels(InputChannels::DEPTH_COMPONENT), GL_FLOAT, data); // oh no
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else if(_isCompressedTexture)
		{
			if(size.x && size.y && imgSize)
				glCompressedTexImage2D(GL_TEXTURE_2D, mipmap_size, detail::ToGLColor(internalFormat), size.x, size.y, 0, static_cast<GLsizei>(imgSize), data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, detail::ToGLColor(internalFormat), size.x, size.y, 0, detail::ToGLinputChannels(format), GL_UNSIGNED_BYTE, data); // oh no
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

	void OpenGLTexture::OnMetaUpdate(const TextureMeta& tex_meta)
	{

		if (_isCompressedTexture)
		{
			UpdateFilter(meta.filter_mode,true);
			UpdateUV(meta.uv_mode);
		}
		else
		{
			UpdateFilter(meta.filter_mode);
			UpdateUV(meta.uv_mode);
			Buffer(nullptr, Size(), tex_meta.format, tex_meta.internal_format);
		}
	}
	void OpenGLTexture::UpdateUV(UVMode uv_mode)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, detail::GLUVMode(uv_mode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, detail::GLUVMode(uv_mode));
		GL_CHECK();
	}
	void OpenGLTexture::UpdateFilter(FilterMode f_mode,const bool& isMipMap)
	{
		if(isMipMap)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, detail::GLFilter(f_mode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, detail::GLFilter(f_mode));
		GL_CHECK();
	}
}
