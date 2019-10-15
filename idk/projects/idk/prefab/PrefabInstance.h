#pragma once

#include <idk.h>
#include <core/Component.h>
#include <prefab/PropertyOverride.h>

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
    };
}