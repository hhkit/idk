#include "stdafx.h"
#include <reflect/reflect.h>

namespace idk
{
    reflect::dynamic PrefabData::FindComponent(string_view name, int nth) const
    {
        const auto comp_type = reflect::get_type(name);
        for (const auto& original_comp : components)
        {
            if (original_comp.type == comp_type)
            {
                if (nth == 0)
                    return original_comp;
                else
                    --nth;
            }
        }
        return reflect::dynamic();
    }

	int PrefabData::GetComponentIndex(string_view name, int nth) const
	{
		int i = 0;
		const auto comp_type = reflect::get_type(name);
		for (const auto& original_comp : components)
		{
			if (original_comp.type == comp_type)
			{
				if (nth == 0)
					return i;
				else
					--nth;
			}
			++i;
		}
		return -1;
	}

    int PrefabData::GetComponentNth(int component_index) const
    {
        if (component_index >= components.size())
            return -1;

        int i = 0;
        const auto comp_type = components[component_index].type;

        while(--component_index >= 0)
        {
            if (components[component_index].type == comp_type)
                ++i;
        }

        return i;
    }
}