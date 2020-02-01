#pragma once
#include <idk.h>
#include <gfx/TextureRenderMeta.h>
#include <res/ResourceExtension.h>
#include <res/compiled_asset_tags.h>

namespace idk
{
	struct CompiledTexture
	{
		Guid guid;//hack
		ColorFormat internal_format;
		UVMode mode;
		FilterMode filter_mode;
		uvec2 size;
		bool is_srgb{};
		unsigned generate_mipmaps{};
		string pixel_buffer;

		EXTENSION(".idds");
	};

	struct CompiledCubeMap
	{
		array<CompiledTexture, 6> textures;
		EXTENSION(".idcb");
	};

	namespace ca_tags
	{
		template<>
		struct prepend_guid<CompiledTexture> :std::true_type {};
	}
}