#pragma once

#include <idk.h>
#include <core/Component.h>
#include <prefab/PropertyOverride.h>
#include <res/ResourceHandle.h>

namespace idk
{
    class Prefab;

    // contains information about prefab base and changes
    class PrefabInstance : public Component<PrefabInstance>
    {
    public:
        RscHandle<Prefab> prefab;
        vector<PropertyOverride> overrides;
        int object_index;
        vector<ComponentNth> removed_components;

        bool HasOverride(string_view component_name, string_view property_path, int component_nth) const;
    };
}