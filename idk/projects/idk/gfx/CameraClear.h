#pragma once

#include <variant>
#include <math/color.h>
#include <res/ResourceHandle.h>

namespace idk
{
	struct DepthOnly {};
    struct DontClear {};
    using CameraClear = std::variant<DontClear, DepthOnly, color, RscHandle<class CubeMap>>;
}