#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <res/ResourceMeta.h>

#include <gfx/TextureRenderMeta.h>

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
		ivec2 Size() const;

		// modifiers
		virtual ivec2 Size(ivec2 newsize);
		virtual void ChangeMode();

		// identifier for ImGUIImage
		virtual void* ID() const { return 0; }
	protected:
		ivec2 _size {};
		void OnMetaUpdate(const TextureMeta&) {};
	};
}