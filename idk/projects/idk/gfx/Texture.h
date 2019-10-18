#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>
#include <util/enum.h>

namespace idk
{
	ENUM(ColorFormat, char,
		R_8,
		R_16,
		R_32F,
		R_64F,
		Rint_8,
		Rint_16,
		Rint_32,
		Rint_64,
		RG_8,
		RGF_16,
		RGB_8,
		RGBF_16,
		RGBF_32,
		RGBA_8,
		RGBAF_16,
		RGBAF_32,
		BGRA_8,
		DEPTH_COMPONENT,
		DXT1,
		DXT3,
		DXT5,
		DXT1_A,
		SRGB ,      
		SRGBA,	    
		SRGB_DXT1 ,	
		SRGB_DXT3 ,	
		SRGB_DXT5 ,	
		SRGBA_DXT1  
	); //TODO remove the SRGB from this list
	inline bool IsSrgb(ColorFormat cf)
	{
		return
			(cf == ColorFormat::SRGB_DXT1 ) |
			(cf == ColorFormat::SRGBA_DXT1) |
			(cf == ColorFormat::SRGB_DXT3 ) |
			(cf == ColorFormat::SRGB_DXT5 ) |
			(cf == ColorFormat::SRGB      ) |
			(cf == ColorFormat::SRGBA	  )
			;
	}

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
        ColorFormat internal_format = ColorFormat::RGBA_8; //Format in GPU
        UVMode      uv_mode = UVMode::Repeat;
		InputChannels format = InputChannels::RGBA;   //Remove, loader determines this
		FilterMode  filter_mode = FilterMode::Linear;
		bool is_srgb   {true};
		bool compressed{true};
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