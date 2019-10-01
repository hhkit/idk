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
		sRGB_8,
		sRGBA_8,
		RGBF_16,
		RGBF_32,
		RGBAF_16,
		RGBAF_32
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
		CMUVMode      uv_mode = CMUVMode::Repeat;
		CMColorFormat internal_format = CMColorFormat::RGBF_32;
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