#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>
#include <gfx/TextureRenderMeta.h>
#include <gfx/TextureInternalFormat.h>

namespace idk
{
	struct CubeMapMeta
	{
		UVMode      uv_mode = UVMode::Clamp;
		ColorFormat internal_format = ColorFormat::Automatic;
		bool is_srgb{ true };
	};

	class CubeMap
		: public MetaResource<CubeMap, CubeMapMeta>
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
		TextureInternalFormat _internal_format{ };
	};
}