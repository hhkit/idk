#pragma once

#include <idk.h>

namespace idk
{
    namespace reflect { class dynamic; }

    struct PrefabData
    {
        vector<reflect::dynamic> components;
        vector<PrefabData> children;
    };
}