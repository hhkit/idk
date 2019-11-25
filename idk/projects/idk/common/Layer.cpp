#include "stdafx.h"
#include "Layer.h"

namespace idk
{
	LayerMask Layer::mask() const
	{
		return LayerMask{ 1 << index };
	}
}
