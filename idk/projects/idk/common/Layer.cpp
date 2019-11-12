#include "stdafx.h"
#include "Layer.h"

namespace idk
{
	LayerMask Layer::mask() const
	{
		return 1 << index;
	}
}
