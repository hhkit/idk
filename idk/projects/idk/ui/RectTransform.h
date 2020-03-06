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
        vec2 anchor_min{ 0, 0 };
        vec2 anchor_max{ 1.0f, 1.0f };
		vec2 anchor_pos{ 0, 0};
        vec2 offset_min{ 0, 0 };
        vec2 offset_max{ 0, 0 };
        vec2 pivot{ 0.5f, 0.5f };
        // transform position: anchored position

        rect _local_rect; // computed, top-left is relative to parent pivot
        mat4 _matrix; // computed screen to normalized viewport matrix
    };
}