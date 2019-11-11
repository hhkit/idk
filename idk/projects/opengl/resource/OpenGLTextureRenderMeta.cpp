#include "pch.h"
#include <opengl/resource/OpenGLTextureRenderMeta.h>
#include <glad/glad.h>

namespace idk::ogl::detail {
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
	unsigned ogl_GraphicsFormat::ToUVMode(const UVMode& uv_mode) noexcept
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
	unsigned ogl_GraphicsFormat::ToColor(const ColorFormat& c) noexcept
	{
		switch (c)
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
	int ogl_GraphicsFormat::ToInputChannels(const InputChannels& ic) noexcept
	{
		switch (ic)
		{
		case InputChannels::RED:  return GL_RED;
		case InputChannels::RG:   return GL_RG;
		case InputChannels::RGB:  return GL_RGB;
		case InputChannels::RGBA: return GL_RGBA;
		case InputChannels::DEPTH_COMPONENT: return GL_DEPTH_COMPONENT;
		default: return 0;
		}
	}
	unsigned ogl_GraphicsFormat::ToFilter(const FilterMode& filter_mode) noexcept
	{
		switch (filter_mode)
		{
		case FilterMode::Linear:       return GL_LINEAR;
		case FilterMode::Nearest:        return GL_NEAREST;
		default: return 0;
		}
	}
}