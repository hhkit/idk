#pragma once
#include <idk.h>
#include <core/Component.h>

namespace idk
{
    class Image
        : public Component<Image>
    {
    public:
        RscHandle<Texture> texture;
    };
}