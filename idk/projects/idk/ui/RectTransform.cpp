#include "stdafx.h"
#include "RectTransform.h"
#include <core/GameObject.h>
#include <app/Application.h>

namespace idk
{

    rect RectTransform::RectInCanvas() const
    {
        idk::rect this_rect;
        if (const auto parent = GetGameObject()->Parent())
        {
            idk::rect parent_rect;
            if (const auto canvas = parent->GetComponent<Canvas>())
            {
                // assume overlay
                parent_rect.size = vec2{ Core::GetSystem<Application>().GetScreenSize() };
            }
            else
            {
                parent_rect = parent->GetComponent<RectTransform>()->RectInCanvas();
            }

            vec2 min = parent_rect.size * anchor_min + offset_min;
            vec2 max = parent_rect.size * anchor_max + offset_max;
            this_rect.position = offset_min;
            this_rect.size = max - min;

        }
		return this_rect;
    }

}