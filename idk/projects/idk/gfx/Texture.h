#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>
#include <util/enum.h>

namespace idk
{
    ENUM(ColorFormat, char,
		RG_8,
	    RGF_16,
        RGB_8,
        RGBA_8,
        RGBF_16,
        RGBF_32,
        RGBAF_16,
        RGBAF_32,
		SRGB,
		DEPTH_COMPONENT,
		DXT1,
		DXT3,
		DXT5

    )

    ENUM(UVMode, char,
         Repeat,
         MirrorRepeat,
         Clamp,
		ClampToBorder
    );

	ENUM(FilterMode, char,
		Linear,
		Nearest
	);

    ENUM(InputChannels, char
         , RED
         , RG
         , RGB
         , RGBA
		, DEPTH_COMPONENT
    );

    struct TextureMeta
    {
        ColorFormat internal_format = ColorFormat::RGBF_32;
        UVMode      uv_mode = UVMode::Repeat;
		InputChannels format = InputChannels::RGBA;
		FilterMode  filter_mode = FilterMode::Linear;
    };

	class Texture
		: public Resource<Texture>
		, public MetaTag<TextureMeta>
	{
	public:
		Texture() = default;
		// destructor
		virtual ~Texture() = default;

		// accessors
		float AspectRatio() const;
		ivec2 Size() const;

		// modifiers
		virtual void Size(ivec2 newsize);
		virtual void ChangeMode();

		// identifier for ImGUIImage
		virtual void* ID() const { return 0; }
	protected:
		ivec2 _size {};
		void OnMetaUpdate(const TextureMeta&) {};
	};
}