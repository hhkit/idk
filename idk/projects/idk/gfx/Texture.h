#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>
#include <util/enum.h>

namespace idk
{
	ENUM(TextureType, char,
		_2D,
		Cube
	);

	ENUM(ColorFormat, char,
		RGB8,
		RGBA8)

	ENUM(UVMode, char,
		Repeat,
		MirroredRepeat,
		Clamp
	);

	struct TextureMeta
	{
		TextureType type          = TextureType::_2D;
		ColorFormat color_format  = ColorFormat::RGB8;
		ivec2       size          = ivec2{ 1024, 1024 };
		bool        render_target = false;
		UVMode      uv_mode       = UVMode::Repeat;
	};

	class Texture
		: public Resource<Texture>
		, public MetaTag<TextureMeta>
	{
	public:
		virtual ~Texture() = default;

		float AspectRatio() const;
	};
}