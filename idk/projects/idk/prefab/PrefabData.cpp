#include "stdafx.h"
#include <reflect/reflect.h>

namespace idk
{
    reflect::dynamic PrefabData::FindComponent(string_view name, int index) const
    {
        auto comp_type = reflect::get_type(name);
        for (auto& original_comp : components)
        {
            if (original_comp.type == comp_type)
            {
                if (index == 0)
                    return original_comp;
                else
                    --index;
            }
        }
        return reflect::dynamic();
    }
}