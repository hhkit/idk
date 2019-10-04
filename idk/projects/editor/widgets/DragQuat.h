#pragma once

#include <idk.h>

namespace idk::ImGuidk
{
    bool DragQuat(const char* label, quat* q, float speed = 0.1f, float min = -360.0f, float max = 360.0f);
}