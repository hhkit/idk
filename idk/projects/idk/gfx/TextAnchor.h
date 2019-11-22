#pragma once

#include <idk.h>
#include <util/enum.h>

namespace idk
{
    ENUM(TextAnchor, char,
        UpperLeft,
        UpperCenter,
        UpperRight,
        MiddleLeft,
        MiddleCenter,
        MiddleRight,
        LowerLeft,
        LowerCenter,
        LowerRight)
}