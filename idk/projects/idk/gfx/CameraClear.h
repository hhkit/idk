#pragma once

#include <variant>
#include <math/color.h>
#include <res/ResourceHandle.h>

namespace idk
{
    struct DepthOnly { int hack; };
    struct DontClear { int hack; };
    using CameraClear = std::variant<color, RscHandle<class CubeMap>, DepthOnly, DontClear>;
}