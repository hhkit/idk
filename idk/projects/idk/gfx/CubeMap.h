#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>
#include <gfx/TextureRenderMeta.h>

namespace idk
{
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