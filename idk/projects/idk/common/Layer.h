#pragma once

#include <core/Component.h>

namespace idk
{
    class Layer : public Component<Layer>
    {
    public:
        int8_t index;
    };
};