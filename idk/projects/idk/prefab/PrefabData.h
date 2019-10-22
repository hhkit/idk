#pragma once

#include <idk.h>

namespace idk
{
    namespace reflect { class dynamic; }

    struct PrefabData
    {
        vector<reflect::dynamic> components;
        int parent_index = -1;

        reflect::dynamic FindComponent(string_view name, int nth) const;
        int GetComponentIndex(string_view name, int nth) const;
        int GetComponentNth(int component_index) const;
    };
}