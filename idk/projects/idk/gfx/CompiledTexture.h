#pragma once
#include <idk.h>
#include <gfx/TextureRenderMeta.h>
#include <res/ResourceExtension.h>

namespace idk
{
	struct CompiledTexture
	{
		ColorFormat internal_format;
		UVMode mode;
		FilterMode filter_mode;
		uivec2 size;
		bool is_srgb{};
		unsigned generate_mipmaps{};
		vector<unsigned char> pixel_buffer;

		EXTENSION(".idds");
	};

	struct CompiledCubeMap
	{
		array<CompiledTexture, 6> textures;
		EXTENSION(".idcb");
	};
}