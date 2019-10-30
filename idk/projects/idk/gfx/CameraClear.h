#pragma once

#include <variant>
#include <math/color.h>
#include <res/ResourceHandle.h>

namespace idk
{
    struct DontClear {};
    using CameraClear = std::variant<DontClear, color, RscHandle<class CubeMap>>;
}