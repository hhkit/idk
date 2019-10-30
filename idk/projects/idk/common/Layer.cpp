#include "stdafx.h"
#include "Layer.h"

namespace idk
{
	Layer::LayerMask Layer::mask() const
	{
		return 1 << index;
	}
}
