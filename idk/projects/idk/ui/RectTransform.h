#pragma once
#include <idk.h>
#include <core/Component.h>
#include <math/rect.h>

namespace idk
{
    class RectTransform
        : public Component<RectTransform>
    {
        vec2 anchor_min;
        vec2 anchor_max;
        vec2 offset_min;
        vec2 offset_max;
        // transform position: anchored position

        rect rect; // computed rect
    };
}