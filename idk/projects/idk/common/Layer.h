#pragma once

#include <core/Component.h>

namespace idk
{
    class Layer : public Component<Layer>
    {
    public:
		using LayerMask = int;
        char index;

		LayerMask mask() const;
    };
};