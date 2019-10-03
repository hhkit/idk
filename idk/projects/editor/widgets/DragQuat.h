#pragma once

#include <idk.h>

namespace idk::ImGuidk
{
    bool DragQuat(const char* label, quat* q, float speed = 0.01f, float min = 0, float max = 0);
}