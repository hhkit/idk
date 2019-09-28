//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_DeleteComponent.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_DeleteComponent.h>

namespace idk {


	CMD_DeleteComponent::CMD_DeleteComponent(Handle<GameObject> gameObject, string component)
		:component_name{ component }
	{
		game_object_handle = gameObject;
	}

	bool CMD_DeleteComponent::execute()
	{
		if (game_object_handle) {
			if (game_object_handle->GetComponent(reflect::get_type(component_name))) { //This is when the gameObjectHandle has a different component handle due to DeleteGameObjectCMD
				serialized_component = serialize_text(*game_object_handle->GetComponent(component_name));
				game_object_handle->RemoveComponent(game_object_handle->GetComponent(reflect::get_type(component_name)));

			}
			return true;
		}
		return false;
	}

	bool CMD_DeleteComponent::undo()
	{
		if (game_object_handle) {
			if (game_object_handle) {
				new_component_handle = game_object_handle->AddComponent(parse_text(serialized_component, reflect::get_type(component_name)));
				return true;
			}
		}

		return false;
	}

}
