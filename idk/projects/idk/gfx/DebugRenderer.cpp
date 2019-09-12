#include "stdafx.h"
#include "DebugRenderer.h"
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

#include <math/matrix_transforms.h>

namespace idk
{
	span<const DebugRenderer::DebugInfo> DebugRenderer::GetWorldDebugInfo() const
	{
		return span<const DebugRenderer::DebugInfo>{debug_info};
	}
}