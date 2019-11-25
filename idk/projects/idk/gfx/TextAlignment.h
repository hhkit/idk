#pragma once

#include <idk.h>
#include <util/enum.h>

namespace idk
{
    ENUM(TextAlignment, char,
        Left = -1,
        Center = 0,
        Right = 1)
}