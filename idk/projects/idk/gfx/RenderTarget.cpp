#include "stdafx.h"
#include "RenderTarget.h"

namespace idk
{
	float RenderTarget::AspectRatio() const
	{
		return s_cast<float>(meta.size.x) / meta.size.y;
	}
}
