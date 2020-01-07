#pragma once
#include <idk.h>
#include <reflect/enum_type.inl>

namespace idk
{
	ENUM(RenderTargetFormat, char,
		RGB
		);

	ENUM(
		AntiAliasing, char
		, None
		, _2_Samples
		, _4_Samples
		, _8_Samples
	);

	struct RenderTargetMeta
	{
		ivec2 size{ 1024, 1024 };
		array<RscHandle<Texture>, 2> textures;
		AntiAliasing anti_aliasing;
		bool enable_mipmapping = false;

		bool is_world_renderer = false;
		bool render_debug = true;
	};

}