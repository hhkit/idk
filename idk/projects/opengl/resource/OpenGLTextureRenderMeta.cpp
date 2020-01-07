#include "pch.h"
#include <opengl/resource/OpenGLTextureRenderMeta.h>
#include <glad/glad.h>

namespace idk::ogl::detail {
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3

	unsigned ogl_GraphicsFormat::ToInternal(const TextureInternalFormat& fmt) noexcept
	{
		switch (fmt)
		{// unsigned normalized data
		case TextureInternalFormat::INTERNAL_FORMAT(R, 8, UNORM)    : return GL_R8;
		case TextureInternalFormat::INTERNAL_FORMAT(RG, 8, UNORM)   : return GL_RG8;
		case TextureInternalFormat::INTERNAL_FORMAT(RGB, 8, UNORM)  : return GL_RGB8;
		case TextureInternalFormat::INTERNAL_FORMAT(RGBA, 8, UNORM) : return GL_RGBA8;
		case TextureInternalFormat::INTERNAL_FORMAT(R, 16, UNORM)   : return GL_R16;
		case TextureInternalFormat::INTERNAL_FORMAT(RG, 16, UNORM)	: return GL_RG16;
		case TextureInternalFormat::INTERNAL_FORMAT(RGB, 16, UNORM)	: return GL_RGB16;
		case TextureInternalFormat::INTERNAL_FORMAT(RGBA, 16, UNORM): return GL_RGBA16;

		// signed normalized data
		case TextureInternalFormat::INTERNAL_FORMAT(R, 8, SNORM)    : return GL_R8_SNORM;
		case TextureInternalFormat::INTERNAL_FORMAT(RG, 8, SNORM)	: return GL_RG8_SNORM;
		case TextureInternalFormat::INTERNAL_FORMAT(RGB, 8, SNORM)	: return GL_RGB8_SNORM;
		case TextureInternalFormat::INTERNAL_FORMAT(RGBA, 8, SNORM)	: return GL_RGBA8_SNORM;
		case TextureInternalFormat::INTERNAL_FORMAT(R, 16, SNORM)	: return GL_R16_SNORM;
		case TextureInternalFormat::INTERNAL_FORMAT(RG, 16, SNORM)	: return GL_RG16_SNORM;
		case TextureInternalFormat::INTERNAL_FORMAT(RGB, 16, SNORM)	: return GL_RGB16_SNORM;
		case TextureInternalFormat::INTERNAL_FORMAT(RGBA, 16, SNORM): return GL_RGBA16_SNORM;

		// unsigned integral data
		case TextureInternalFormat::INTERNAL_FORMAT(R, 8, UI)     : return GL_R8UI;
		case TextureInternalFormat::INTERNAL_FORMAT(RG, 8, UI)	  : return GL_RG8UI;
		case TextureInternalFormat::INTERNAL_FORMAT(RGB, 8, UI)	  : return GL_RGB8UI;
		case TextureInternalFormat::INTERNAL_FORMAT(RGBA, 8, UI)  : return GL_RGBA8UI;
		case TextureInternalFormat::INTERNAL_FORMAT(R, 16, UI)	  : return GL_R16UI;
		case TextureInternalFormat::INTERNAL_FORMAT(RG, 16, UI)	  : return GL_RG16UI;
		case TextureInternalFormat::INTERNAL_FORMAT(RGB, 16, UI)  : return GL_RGB16UI;
		case TextureInternalFormat::INTERNAL_FORMAT(RGBA, 16, UI) : return GL_RGBA16UI;
		case TextureInternalFormat::INTERNAL_FORMAT(R, 32, UI)	  : return GL_R32UI;
		case TextureInternalFormat::INTERNAL_FORMAT(RG, 32, UI)	  : return GL_RG32UI;
		case TextureInternalFormat::INTERNAL_FORMAT(RGB, 32, UI)  : return GL_RGB32UI;
		case TextureInternalFormat::INTERNAL_FORMAT(RGBA, 32, UI) : return GL_RGBA32UI;

		// signed intergral data
		case TextureInternalFormat::INTERNAL_FORMAT(R, 8, I)     : return GL_R8I;
		case TextureInternalFormat::INTERNAL_FORMAT(RG, 8, I)	 : return GL_RG8I;
		case TextureInternalFormat::INTERNAL_FORMAT(RGB, 8, I)	 : return GL_RGB8I;
		case TextureInternalFormat::INTERNAL_FORMAT(RGBA, 8, I)	 : return GL_RGBA8I;
		case TextureInternalFormat::INTERNAL_FORMAT(R, 16, I)	 : return GL_R16I;
		case TextureInternalFormat::INTERNAL_FORMAT(RG, 16, I)	 : return GL_RG16I;
		case TextureInternalFormat::INTERNAL_FORMAT(RGB, 16, I)	 : return GL_RGB16I;
		case TextureInternalFormat::INTERNAL_FORMAT(RGBA, 16, I) : return GL_RGBA16I;
		case TextureInternalFormat::INTERNAL_FORMAT(R, 32, I)	 : return GL_R32I;
		case TextureInternalFormat::INTERNAL_FORMAT(RG, 32, I)	 : return GL_RG32I;
		case TextureInternalFormat::INTERNAL_FORMAT(RGB, 32, I)	 : return GL_RGB32I;
		case TextureInternalFormat::INTERNAL_FORMAT(RGBA, 32, I) : return GL_RGBA32I;

		// floating point data
		case TextureInternalFormat::INTERNAL_FORMAT(R, 16, F)    : return GL_R16F;
		case TextureInternalFormat::INTERNAL_FORMAT(RG, 16, F)	 : return GL_RG16F;
		case TextureInternalFormat::INTERNAL_FORMAT(RGB, 16, F)	 : return GL_RGB16F;
		case TextureInternalFormat::INTERNAL_FORMAT(RGBA, 16, F) : return GL_RGBA16F;
		case TextureInternalFormat::INTERNAL_FORMAT(R, 32, F)	 : return GL_R32F;
		case TextureInternalFormat::INTERNAL_FORMAT(RG, 32, F)	 : return GL_RG32F;
		case TextureInternalFormat::INTERNAL_FORMAT(RGB, 32, F)	 : return GL_RGB32F;
		case TextureInternalFormat::INTERNAL_FORMAT(RGBA, 32, F) : return GL_RGBA32F;
											
		// compressed				
		case TextureInternalFormat::RGB_DXT1  : return GL_COMPRESSED_RGB;
		case TextureInternalFormat::RGBA_DXT1 : return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		case TextureInternalFormat::RGBA_DXT3 : return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		case TextureInternalFormat::RGBA_DXT5 : return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

		// srgb
		case TextureInternalFormat::SRGB_8:         return GL_COMPRESSED_SRGB8_ETC2;
		case TextureInternalFormat::SRGB_8_ALPHA_8: return GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;

		// compressed srgb
		#pragma message("please remember to update glad to have the ARB_texture_view extension ")

		/*
		case TextureInternalFormat::SRGB_DXT1  : return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
		case TextureInternalFormat::SRGBA_DXT1  : return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
		case TextureInternalFormat::SRGBA_DXT3 : return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
		case TextureInternalFormat::SRGBA_DXT5 : return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
		*/
				// depth buffer
		case TextureInternalFormat::DEPTH_16: 
			return GL_DEPTH_ATTACHMENT;

		case TextureInternalFormat::DEPTH_24:
		case TextureInternalFormat::DEPTH_24_STENCIL_8:
			return GL_DEPTH24_STENCIL8;

		case TextureInternalFormat::DEPTH_32: 
		case TextureInternalFormat::DEPTH_32_F:
		case TextureInternalFormat::DEPTH_32_F_STENCIL_8:
			return GL_DEPTH32F_STENCIL8;
		default:
			return 0;
		}
	}

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

	unsigned ogl_GraphicsFormat::ToFilter(const FilterMode& filter_mode) noexcept
	{
		switch (filter_mode)
		{
		case FilterMode::Linear:       return GL_LINEAR;
		case FilterMode::Nearest:      return GL_NEAREST;
		default: return 0;
		}
	}
}