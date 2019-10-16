#pragma once

#include <core/Component.h>

namespace idk
{
    class Layer : public Component<Layer>
    {
    public:
        char index;
    };
};