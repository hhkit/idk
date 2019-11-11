#include "pch.h"
#include "OpenGLFontAtlas.h"

#include <opengl/resource/OpenGLTextureRenderMeta.h>

namespace idk::ogl {

	OpenGLFontAtlas::OpenGLFontAtlas()
	{
		glGenTextures(1, &_id);
		//*texture = OpenGLTexture{ _id, _size };
		glBindTexture(GL_TEXTURE_2D, _id);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		UpdateFilter(meta.filter_mode);
		UpdateUV(meta.uv_mode);
		Buffer(nullptr, _size);
	}

	OpenGLFontAtlas::OpenGLFontAtlas(const bool& compressed)
	{
		glGenTextures(1, &_id);
		//*texture = OpenGLTexture{ _id, _size };
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
		: FontAtlas{ std::move(rhs) }, 
		_id{ rhs._id }, 
		_isCompressedTexture{ rhs._isCompressedTexture },
		texture{ rhs.texture }
	{
		rhs._id = 0;
	}

	OpenGLFontAtlas& OpenGLFontAtlas::operator=(OpenGLFontAtlas&& rhs)
	{
		// TODO: insert return statement here
		FontAtlas::operator=(std::move(rhs));
		std::swap(_id, rhs._id);
		std::swap(_isCompressedTexture, rhs._isCompressedTexture);
		std::swap(texture, rhs.texture);
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

	void OpenGLFontAtlas::Buffer(void* data, ivec2 size, InputChannels format, ColorFormat internalFormat, const unsigned& mipmap_size, const float& imgSize)
	{
		_size = size;
		glBindTexture(GL_TEXTURE_2D, _id);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (internalFormat == ColorFormat::DEPTH_COMPONENT)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, detail::ogl_GraphicsFormat::ToColor(internalFormat), size.x, size.y, 0, detail::ogl_GraphicsFormat::ToInputChannels(InputChannels::DEPTH_COMPONENT), GL_UNSIGNED_BYTE, data); // oh no
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else if (_isCompressedTexture)
		{
			if (size.x && size.y && imgSize)
				glCompressedTexImage2D(GL_TEXTURE_2D, mipmap_size, detail::ogl_GraphicsFormat::ToColor(internalFormat), size.x, size.y, 0, static_cast<GLsizei>(imgSize), data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, detail::ogl_GraphicsFormat::ToColor(internalFormat), size.x, size.y, 0, detail::ogl_GraphicsFormat::ToInputChannels(format), GL_UNSIGNED_BYTE, data); // oh no
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

	void OpenGLFontAtlas::UpdateUV(UVMode uv_mode)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, detail::ogl_GraphicsFormat::ToUVMode(uv_mode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, detail::ogl_GraphicsFormat::ToUVMode(uv_mode));
		GL_CHECK();
	}

	void OpenGLFontAtlas::UpdateFilter(FilterMode f_mode, const bool& isMipMap)
	{
		if (isMipMap)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, detail::ogl_GraphicsFormat::ToFilter(f_mode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, detail::ogl_GraphicsFormat::ToFilter(f_mode));
		GL_CHECK();
	}

	RscHandle<Texture> OpenGLFontAtlas::Tex() const noexcept
	{
		return RscHandle<Texture>{texture};
	}
}