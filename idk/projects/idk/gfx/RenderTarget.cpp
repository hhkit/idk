#include "stdafx.h"
#include "RenderTarget.h"

namespace idk
{
	float RenderTarget::AspectRatio() const
	{
		return s_cast<float>(meta.size.x) / meta.size.y;
	}
	RscHandle<Texture> RenderTarget::GetColorBuffer()
	{
		return meta.textures[kColorIndex];
	}
	RscHandle<Texture> RenderTarget::GetDepthBuffer()
	{
		return meta.textures[kDepthIndex];
	}
}
