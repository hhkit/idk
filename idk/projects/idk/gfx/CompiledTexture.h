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
		bool force_uncompiled{};
		unsigned generate_mipmaps{};
		bool wait_loaded=false;
		string pixel_buffer;

		EXTENSION(".idds");
	};

	struct CompiledCubeMap
	{
		array<CompiledTexture, 6> textures;
		EXTENSION(".idcb");
	};

	struct CompiledFontAtlas
	{
		Guid guid;//hack
		ColorFormat internal_format;
		UVMode mode;
		FilterMode filter_mode;
		uvec2 size;
		bool is_srgb{};
		string pixel_buffer;
		
		//Font data itself
		unsigned font_size = 48;
		//Characters information
		vector<CharacterInfo> char_map;
		// max y above baseline
		float ascender;
		// max y below baseline
		float descender;

		//uvec2 size{};

		EXTENSION(".ifat");
	};

	namespace ca_tags
	{
		template<>
		struct prepend_guid<CompiledTexture> :std::true_type {};
	}
}