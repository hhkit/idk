#pragma once
#include <idk.h>
#include <gfx/vertex_descriptor.h>
#include <gfx/TextureRenderMeta.h>
#include <res/ResourceExt.h>
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

		EXTENSION(".idds");
	};

	struct CompiledBuffer
	{
		vector<vtx::Attrib>   attribs;
		vector<unsigned char> data_buffer;
	};

	struct CompiledMesh
	{
		vector<CompiledBuffer> buffers;

		EXTENSION(".idmsh");
	};

	// Skeleton resource

	// Animation resource

	struct CompiledCubeMap
	{
		array<CompiledTexture, 6> textures;
		EXTENSION(".idcb");
	};
}