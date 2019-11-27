#pragma once
#include <idk.h>
#include <core/Component.h>
#include <math/rect.h>

namespace idk
{
    class RectTransform
        : public Component<RectTransform>
    {
    public:
        vec2 anchor_min;
        vec2 anchor_max;
        vec2 offset_min;
        vec2 offset_max;
        vec2 pivot;
        // transform position: anchored position

        rect rect; // computed local rect

        idk::rect RectInCanvas() const;
    };
}