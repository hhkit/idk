#pragma once
#include <idk.h>
#include <core/Component.h>
#include <ui/UISystem.h>

namespace idk
{
    class AspectRatioFitter
        : public Component<AspectRatioFitter>
    {
    public:
        float aspect_ratio = 1.0f;
    };
}