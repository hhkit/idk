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
		UVMode      uv_mode       = UVMode::Repeat;
	};

	class Texture
		: public Resource<Texture>
		, public MetaTag<TextureMeta>
	{
	public:
		// destructor
		virtual ~Texture() = default;

		// accessors
		float AspectRatio() const;
		ivec2 Size() const;

		// modifiers
		void Size(ivec2 newsize);

		// imgui identifier
		virtual void* ID() const = 0;
	protected:
		ivec2 _size {};
	};
}