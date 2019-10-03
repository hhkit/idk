#pragma once

#include <idk.h>

namespace idk::ImGuidk
{
    bool DragVec3(const char* label, vec3* vec, float speed = 0.01f, float min = 0, float max = 0);
}