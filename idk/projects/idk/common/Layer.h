#pragma once

#include <core/Component.h>

namespace idk
{
    using LayerMask = uint32_t;

    class Layer : public Component<Layer>
    {
    public:
        char index;

		LayerMask mask() const;
    };
};