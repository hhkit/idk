#include "pch.h"
#include "OpenGLCubemap.h"

#include <idk.h>
#include <core/Core.h>
#include <res/ResourceManager.h>

namespace idk::ogl
{
	namespace detail
	{
		auto GLUVMode(CMUVMode uv_mode) -> GLenum
		{
			switch (uv_mode)
			{
			case CMUVMode::Repeat:       return GL_REPEAT;
			case CMUVMode::Clamp:        return GL_CLAMP_TO_EDGE;
			case CMUVMode::MirrorRepeat: return GL_MIRRORED_REPEAT;
			default: return 0;
			}
		}

		auto ToGLColor(CMColorFormat f)-> GLenum
		{
			switch (f)
			{
			case CMColorFormat::sRGB_8:  return GL_RGB8;
			case CMColorFormat::sRGBA_8: return GL_RGBA8;
			case CMColorFormat::RGBF_16: return GL_RGB16F;
			case CMColorFormat::RGBF_32: return GL_RGB32F;
			case CMColorFormat::RGBAF_16: return GL_RGBA16F;
			case CMColorFormat::RGBAF_32: return GL_RGBA32F;
			case CMColorFormat::SRGB:   return GL_SRGB;
			default: return 0;
			}
		}

		auto ToGLinputChannels(CMInputChannels i)-> GLint
		{
			switch (i)
			{
			case CMInputChannels::RED:  return GL_RED;
			case CMInputChannels::RG:   return GL_RG;
			case CMInputChannels::RGB:  return GL_RGB;
			case CMInputChannels::RGBA: return GL_RGBA;
			default: return 0;
			}
		}


		/*
		
			Texture target	Orientation
			0	GL_TEXTURE_CUBE_MAP_POSITIVE_X	Right
			1	GL_TEXTURE_CUBE_MAP_NEGATIVE_X	Left
			2	GL_TEXTURE_CUBE_MAP_POSITIVE_Y	Top
			3	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	Bottom
			4	GL_TEXTURE_CUBE_MAP_POSITIVE_Z	Back
			5	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	Front
		*/

	}
	OpenGLCubemap::OpenGLCubemap()
	{
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _id);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 9);

		meta.uv_mode = CMUVMode::Clamp;
		UpdateUV(meta.uv_mode);
		
		for (unsigned int i = 0; i < 6; i++)
			Buffer(i,nullptr, _size);

		//Todo GET FILE HANDLE
		//Core::GetResourceManager().GetFactory<ShaderProgramFactory>().Create();

		// generate convoluted cubemap
		glGenTextures(2, _convoluted_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _convoluted_id[0]);
		for (int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 9);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	OpenGLCubemap::OpenGLCubemap(OpenGLCubemap&& rhs)
		: CubeMap{ std::move(rhs) }, _id{ rhs._id }
	{
		rhs._id = 0;
	}

	OpenGLCubemap& OpenGLCubemap::operator=(OpenGLCubemap&& rhs)
	{
		// TODO: insert return statement here
		CubeMap::operator=(std::move(rhs));
		std::swap(_id, rhs._id);
		return *this;
	}

	OpenGLCubemap::~OpenGLCubemap()
	{
		glDeleteTextures(1, &_id);
	}

	void OpenGLCubemap::Bind()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _id);
	}

	void OpenGLCubemap::BindToUnit(GLuint unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _id);
	}

	void OpenGLCubemap::BindConvolutedToUnit(GLuint texture_unit)
	{
		glActiveTexture(GL_TEXTURE0 + texture_unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _convoluted_id[0]);
		//GL_CHECK();
	}

	void OpenGLCubemap::Buffer(unsigned int face_value,void* data, ivec2 size, CMInputChannels format, CMColorFormat colorFormat)
	{
		format;

		_size = size;
		glBindTexture(GL_TEXTURE_CUBE_MAP, _id);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_value, 0, detail::ToGLColor(colorFormat), size.x, size.y, 0, detail::ToGLinputChannels(format), GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		GL_CHECK();
	}


	void OpenGLCubemap::Size(ivec2 new_size)
	{
		CubeMap::Size(new_size);
		for (unsigned int i = 0; i < 6; i++)
			Buffer(i, nullptr, _size);
	}

	void* OpenGLCubemap::ID() const
	{
		return r_cast<void*>(_id);
	}

	span<const GLuint> OpenGLCubemap::ConvolutedID() const
	{
		return span<const GLuint>{_convoluted_id};
	}

	void OpenGLCubemap::OnMetaUpdate(const CubeMapMeta& tex_meta)
	{
		UpdateUV(tex_meta.uv_mode);
	}
	void OpenGLCubemap::UpdateUV(CMUVMode uv_mode)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, detail::GLUVMode(uv_mode));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, detail::GLUVMode(uv_mode));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, detail::GLUVMode(uv_mode));
		GL_CHECK();
	}

};