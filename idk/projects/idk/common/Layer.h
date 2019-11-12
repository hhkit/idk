#pragma once

#include <core/Component.h>

namespace idk
{
    class Layer : public Component<Layer>
    {
    public:
		using LayerMask = uint32_t;
        char index;

		LayerMask mask() const;
    };
};