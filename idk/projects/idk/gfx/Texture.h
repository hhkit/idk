#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>

#include <gfx/TextureRenderMeta.h>
#include <gfx/TextureInternalFormat.h>

namespace idk
{

	struct TextureMeta
	{
		ColorFormat internal_format = ColorFormat::Automatic; //Format in GPU
		UVMode      uv_mode         = UVMode::Repeat;
		FilterMode  filter_mode     = FilterMode::Linear;
		bool        is_srgb         = true;
		unsigned    mipmap_level    = 0;
	};

	class Texture
		: public MetaResource<Texture, TextureMeta>
	{
	public:
		Texture() = default;
		// destructor
		virtual ~Texture() = default;

		// accessors
		float AspectRatio() const;
		uivec2 Size() const;
		TextureInternalFormat InternalFormat() const;
		idk::FilterMode Filter() const;
		bool IsDepthTexture() const;

		// modifiers
		virtual uivec2 Size(uivec2 newsize);

		// identifier for ImGUIImage
		virtual void* ID() const { return 0; }
	protected:
		uivec2 _size {512,512};
		TextureInternalFormat _internal_format = TextureInternalFormat::RGBA_16_F;
		FilterMode _filter_mode;
	};
}