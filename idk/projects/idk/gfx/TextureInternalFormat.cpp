#include "stdafx.h"
#include "TextureInternalFormat.h"

namespace idk
{
	TextureInternalFormat ToInternalFormat(ColorFormat fmt, bool compressed)
	{
		switch (fmt)
		{
		case ColorFormat::Alpha_8:   return TextureInternalFormat::R_8;
		case ColorFormat::RGB_24bit: return compressed ? TextureInternalFormat::SRGB_DXT1 : TextureInternalFormat::RGB_DXT1;
		case ColorFormat::RGBA_32bit:return compressed ? TextureInternalFormat::SRGBA_DXT5 : TextureInternalFormat::RGBA_DXT5;
		case ColorFormat::RGB_16bit: return compressed ? TextureInternalFormat::SRGB_DXT1 : TextureInternalFormat::RGB_DXT1;
		case ColorFormat::R_16bit:   return TextureInternalFormat::R_16;
		case ColorFormat::R_8:       return TextureInternalFormat::R_8;
		default:
			break;
		}
		return TextureInternalFormat::None;
	}

	TextureInternalFormat ToInternalFormat(DepthBufferMode mode, bool stencil)
	{
		switch (mode)
		{
		case DepthBufferMode::NoDepth:   return TextureInternalFormat::None;
		case DepthBufferMode::Depth16:   return TextureInternalFormat::DEPTH_16;
		case DepthBufferMode::Depth2432: return stencil ? TextureInternalFormat::DEPTH_32_F_STENCIL_8 : TextureInternalFormat::DEPTH_32_F;
		}
		return TextureInternalFormat::None;
	}
}
