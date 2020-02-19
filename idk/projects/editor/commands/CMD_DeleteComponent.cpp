//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_DeleteComponent.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		26 OCT 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_DeleteComponent.h>
#include <prefab/PrefabUtility.h>
#include <prefab/Prefab.h>

namespace idk 
{
	CMD_DeleteComponent::CMD_DeleteComponent(Handle<GameObject> gameObject, GenericHandle component)
		: component{ component }, component_name{ (*component).type.name() }, component_data{ (*component).copy() }
	{
		game_object_handle = gameObject;
	}

	CMD_DeleteComponent::CMD_DeleteComponent(Handle<GameObject> gameObject, string name)
		: component{ gameObject->GetComponent(name) }, component_name{ (*component).type.name() }, component_data{ (*component).copy() }
	{
		game_object_handle = gameObject;
	}

	bool CMD_DeleteComponent::execute()
	{
		if (!game_object_handle)
			return false;

		if (const auto prefab_inst = game_object_handle->GetComponent<PrefabInstance>())
		{
			int nth = -1;
			for (const auto& c : game_object_handle->GetComponents())
			{
				if (c.type == component.type)
					++nth;
				if (c == component)
					break;
			}

			if (nth >= 0)
			{
				game_object_handle->RemoveComponent(component);
				if (PrefabUtility::RecordPrefabInstanceRemoveComponent(game_object_handle, component_name, nth))
				{
					prefab_component_index = prefab_inst->prefab->data[prefab_inst->object_index]
						.GetComponentIndex(component_name, prefab_inst->removed_components.back().component_nth);
				}
				return true;
			}
		}

		game_object_handle->RemoveComponent(component);
		return true;
	}

	bool CMD_DeleteComponent::undo()
	{
		if (!game_object_handle)
			return false;

		if (const auto prefab_inst = game_object_handle->GetComponent<PrefabInstance>() && prefab_component_index >= 0)
		{
			PrefabUtility::RevertRemovedComponent(game_object_handle, prefab_component_index, component);
			*component = component_data;
		}
		else
			game_object_handle->AddComponent(component, component_data);

		return true;
	}

}
