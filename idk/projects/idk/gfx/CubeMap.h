#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>
#include <util/enum.h>

namespace idk
{
	ENUM(TextureTarget, int,
		PosX, NegX,
		PosY, NegY,
		PosZ, NegZ
		);

	ENUM(CMColorFormat, char,
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
		RGBA_8,
		RGBF_16,
		RGBF_32,
		RGBAF_16,
		RGBAF_32,
		BGRA_8,
		RUI_32,
		DEPTH_COMPONENT,
		DXT1,
		DXT3,
		DXT5,
		DXT1_A,
		SRGB,
		SRGBA,
		SRGB_DXT1,
		SRGB_DXT3,
		SRGB_DXT5,
		SRGBA_DXT1
	)

	ENUM(CMInputChannels, char
		, RED
		, RG
		, RGB
		, RGBA
	);

	ENUM(CMUVMode, char,
		Repeat,
		MirrorRepeat,
		Clamp
	);

	struct CubeMapMeta
	{
		CMUVMode      uv_mode = CMUVMode::Clamp;
		CMColorFormat internal_format = CMColorFormat::RGBAF_16;
		bool is_srgb{ true };
		bool compressed{ true };
	};

	class CubeMap
		: public Resource<CubeMap>
		, public MetaTag<CubeMapMeta>
	{
	public:
		CubeMap() = default;
		virtual void* ID() const{ return 0; };
		
		//Dtor
		virtual ~CubeMap() = default;

		// accessors
		float AspectRatio() const;
		ivec2 Size() const;

		// modifiers
		virtual void Size(ivec2 newsize);

	protected:
		//Size for each texture in the cubemap
		ivec2 _size{};
		void OnMetaUpdate(const CubeMapMeta&) {};
	};
}