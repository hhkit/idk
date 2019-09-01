#pragma once
#include <res/Resource.h>
#include <res/ResourceMeta.h>
#include <gfx/Texture.h>

namespace idk
{
	struct RenderTargetMeta
	{
		ivec2 size {1024, 1024};
		vector<RscHandle<Texture>> textures;
		RscHandle<Texture> depth_buffer;
	};

	class RenderTarget
		: public Resource<RenderTarget>
		, public MetaTag<RenderTargetMeta>
	{
		float AspectRatio() const;
	};

}