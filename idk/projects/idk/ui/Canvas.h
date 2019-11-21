#pragma once
#include <idk.h>
#include <core/Component.h>

namespace idk
{
    class Canvas
        : public Component<Canvas>
    {
    public:
        RscHandle<RenderTarget> render_target;
    };
}