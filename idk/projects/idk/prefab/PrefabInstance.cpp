#include "stdafx.h"
#include "PrefabInstance.h"
#include <core/GameObject.inl>

namespace idk
{
    static int _instance_component_nth_to_prefab_data_component_nth(Handle<GameObject> target, string_view component_name, int component_nth)
    {
        // offset nth by removed components to sync that to prefab data

        auto prefab_inst = target->GetComponent<PrefabInstance>();
        IDK_ASSERT(prefab_inst);

        auto& removed = prefab_inst->removed_components;
        std::sort(removed.begin(), removed.end(), [](const ComponentNth& a, const ComponentNth& b) {
            int diff = a.component_name.compare(b.component_name);
            return diff == 0 ? (a.component_nth < b.component_nth) : (diff < 0);
        });

        for (const auto& i : removed)
        {
            if (component_name == i.component_name && i.component_nth <= component_nth)
                ++component_nth;
        }

        return component_nth;
    }

    bool PrefabInstance::HasOverride(string_view component_name, string_view property_path, int component_nth) const
    {
        component_nth = _instance_component_nth_to_prefab_data_component_nth(GetGameObject(), component_name, component_nth);
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