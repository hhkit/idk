#pragma once
#include <idk.h>
#include <gfx/TextureRenderMeta.h>
namespace idk
{
	struct CompiledTexture
	{
		ColorFormat  internal_format;
		UVMode       uv_mode;
		FilterMode   filter_mode;
		ivec2        size;
		bool         is_srgb;

		vector<unsigned char> pixel_buffer;
	};

	struct CompiledBuffer
	{
		// attributes
		// buffer

	};

	struct CompiledMesh
	{
		vector<CompiledBuffer> buffers;
	};

	// Skeleton resource

	// Animation resource

	struct CompiledCubeMap
	{
		array<CompiledTexture, 6> textures;
	};
}