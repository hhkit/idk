#pragma once

#include <idk.h>
#include <math/rect.h>

namespace idk::ImGuidk
{
    bool DragRect(const char* label, rect* vec, float speed = 0.01f, float min = 0, float max = 0);
}