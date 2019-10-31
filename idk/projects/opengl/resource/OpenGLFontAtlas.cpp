#include "pch.h"
#include "OpenGLFontAtlas.h"

namespace idk::ogl {

	namespace detail
	{
		auto GLFilter(FontFilterMode filter_mode) -> GLenum
		{
			switch (filter_mode)
			{
			case FontFilterMode::Linear:       return GL_LINEAR;
			case FontFilterMode::Nearest:        return GL_NEAREST;
			default: return 0;
			}
		}

		auto GLUVMode(FontUVMode uv_mode) -> GLenum
		{
			switch (uv_mode)
			{
			case FontUVMode::Repeat:       return GL_REPEAT;
			case FontUVMode::Clamp:        return GL_CLAMP_TO_EDGE;
			case FontUVMode::MirrorRepeat: return GL_MIRRORED_REPEAT;
			case FontUVMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
			default: return 0;
			}
		}
		#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
		#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
		#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
		auto ToGLColor(FontColorFormat f)-> GLenum
		{
			switch (f)
			{
			case FontColorFormat::RUI_32: return GL_R32UI;
			case FontColorFormat::RG_8: return GL_RG;
			case FontColorFormat::RGF_16: return GL_RG16F;
			case FontColorFormat::RGB_8:  return GL_RGB8;
			case FontColorFormat::RGBA_8: return GL_RGBA8;
			case FontColorFormat::RGBF_16: return GL_RGB16F;
			case FontColorFormat::RGBF_32: return GL_RGB32F;
			case FontColorFormat::RGBAF_16: return GL_RGBA16F;
			case FontColorFormat::RGBAF_32: return GL_RGBA32F;
			case FontColorFormat::SRGB: return GL_SRGB;
			case FontColorFormat::DEPTH_COMPONENT: return GL_DEPTH_COMPONENT;
			case FontColorFormat::DXT1: return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			case FontColorFormat::DXT3: return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			case FontColorFormat::DXT5: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			default: return 0;
			}
		}

		auto ToGLinputChannels(FontInputChannels i)-> GLint
		{
			switch (i)
			{
			case FontInputChannels::RED:  return GL_RED;
			case FontInputChannels::RG:   return GL_RG;
			case FontInputChannels::RGB:  return GL_RGB;
			case FontInputChannels::RGBA: return GL_RGBA;
			case FontInputChannels::DEPTH_COMPONENT: return GL_DEPTH_COMPONENT;
			default: return 0;
			}
		}
	}

	OpenGLFontAtlas::OpenGLFontAtlas()
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		UpdateFilter(meta.filter_mode);
		UpdateUV(meta.uv_mode);
		Buffer(nullptr, _size);
	}

	OpenGLFontAtlas::OpenGLFontAtlas(const bool& compressed)
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (compressed)
		{
			UpdateFilter(meta.filter_mode, true);
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

	OpenGLFontAtlas::OpenGLFontAtlas(OpenGLFontAtlas&& rhs)
		: FontAtlas{ std::move(rhs) }, _id{ rhs._id }, _isCompressedTexture{ rhs._isCompressedTexture }
	{
		rhs._id = 0;
	}

	OpenGLFontAtlas& OpenGLFontAtlas::operator=(OpenGLFontAtlas&& rhs)
	{
		// TODO: insert return statement here
		FontAtlas::operator=(std::move(rhs));
		std::swap(_id, rhs._id);
		std::swap(_isCompressedTexture, rhs._isCompressedTexture);
		return *this;
	}

	OpenGLFontAtlas::~OpenGLFontAtlas()
	{
		glDeleteTextures(1, &_id);
	}

	void OpenGLFontAtlas::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, _id);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	void OpenGLFontAtlas::BindToUnit(GLuint texture_unit)
	{
		glActiveTexture(GL_TEXTURE0 + texture_unit);
		glBindTexture(GL_TEXTURE_2D, _id);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	void OpenGLFontAtlas::Buffer(void* data, ivec2 size, FontInputChannels format, FontColorFormat internalFormat, const unsigned& mipmap_size, const float& imgSize)
	{
		_size = size;
		glBindTexture(GL_TEXTURE_2D, _id);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (internalFormat == FontColorFormat::DEPTH_COMPONENT)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, detail::ToGLColor(internalFormat), size.x, size.y, 0, detail::ToGLinputChannels(FontInputChannels::DEPTH_COMPONENT), GL_UNSIGNED_BYTE, data); // oh no
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else if (_isCompressedTexture)
		{
			if (size.x && size.y && imgSize)
				glCompressedTexImage2D(GL_TEXTURE_2D, mipmap_size, detail::ToGLColor(internalFormat), size.x, size.y, 0, static_cast<GLsizei>(imgSize), data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, detail::ToGLColor(internalFormat), size.x, size.y, 0, detail::ToGLinputChannels(format), GL_UNSIGNED_BYTE, data); // oh no
			//glGenerateMipmap(GL_TEXTURE_2D);
		}

		//glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		// TODO: fix internal format
		GL_CHECK();
	}

	void OpenGLFontAtlas::Size(ivec2 new_size)
	{
		FontAtlas::Size(new_size);
		if (!_isCompressedTexture)
			Buffer(nullptr, _size);
		//return FontAtlas::Size();
	}

	void* OpenGLFontAtlas::ID() const
	{
		return r_cast<void*>(_id);
	}

	void OpenGLFontAtlas::OnMetaUpdate(const FontAtlasMeta& font_meta)
	{
		if (_isCompressedTexture)
		{
			UpdateFilter(meta.filter_mode, true);
			UpdateUV(meta.uv_mode);
		}
		else
		{
			UpdateFilter(meta.filter_mode);
			UpdateUV(meta.uv_mode);
			Buffer(nullptr, Size(), font_meta.format, font_meta.internal_format);
		}
	}

	void OpenGLFontAtlas::UpdateUV(FontUVMode uv_mode)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, detail::GLUVMode(uv_mode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, detail::GLUVMode(uv_mode));
		GL_CHECK();
	}

	void OpenGLFontAtlas::UpdateFilter(FontFilterMode f_mode, const bool& isMipMap)
	{
		if (isMipMap)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, detail::GLFilter(f_mode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, detail::GLFilter(f_mode));
		GL_CHECK();
	}

}