#include "pch.h"
#include "CMD_ModifyProperty.h"
#include <script/ManagedObj.inl>
#include <script/MonoBehavior.h>
#include <serialize/text.inl>
#include <ds/result.inl>
#include <res/ResourceHandle.inl>
#include <util/property_path.h>
#include <prefab/PrefabUtility.h>
#include <IncludeComponents.h>


namespace idk
{

    CMD_ModifyProperty::CMD_ModifyProperty(GenericHandle component, string_view property_path, reflect::dynamic old_value, reflect::dynamic new_value)
        : component{ component }, property_path{ property_path }, old_value{ old_value }, new_value{ new_value }
    {
        game_object_handle = component.visit([](auto h)
        {
            if constexpr (!std::is_same_v<decltype(h), Handle<GameObject>>)
                return h->GetGameObject();
            else
                return Handle<GameObject>();
        });
    }

	CMD_ModifyProperty::CMD_ModifyProperty(reflect::dynamic object, reflect::dynamic old_value)
        : object{ object }, new_value{object.copy()}, old_value{old_value}
	{
	}

	bool CMD_ModifyProperty::execute()
    {
        if (!object.valid())
        {
            if (!component)
                return false;
            assign_property_path(*component, property_path, new_value);
            if (const auto prefab_inst = game_object_handle->GetComponent<PrefabInstance>())
            {
                overrides_old = prefab_inst->overrides;
                PrefabUtility::RecordPrefabInstanceChange(game_object_handle, component, property_path, new_value);
            }
        }
        else
        {
            object = new_value;
        }
        return true;
    }

    bool CMD_ModifyProperty::undo()
    {
        if (!object.valid())
        {
            if (!component)
                return false;
            assign_property_path(*component, property_path, old_value);
            if (const auto prefab_inst = game_object_handle->GetComponent<PrefabInstance>())
                prefab_inst->overrides = overrides_old;
        }
        else
            object = old_value;
        return true;
    }

}
