#include "stdafx.h"
#include "PrefabInstance.h"

namespace idk
{
    bool PrefabInstance::HasOverride(string_view component_name, string_view property_path, int component_nth) const
    {
        for (const auto& ov : overrides)
        {
            if (ov.component_name == component_name &&
                ov.property_path == property_path &&
                ov.component_nth == component_nth)
            {
                return true;
            }
        }
        return false;
    }
}