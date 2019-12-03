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
		ivec2 size;
		bool is_srgb;
		vector<unsigned char> pixel_buffer;

		EXTENSION(".idds");
	};

	struct CompiledCubeMap
	{
		array<CompiledTexture, 6> textures;
		EXTENSION(".idcb");
	};
}