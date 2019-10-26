//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_AddComponent.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_AddComponent.h>

namespace idk {


	CMD_AddComponent::CMD_AddComponent(Handle<GameObject> gameObject, string component)
		:component_name{ component }
	{
		game_object_handle = gameObject;
	}

	CMD_AddComponent::CMD_AddComponent(Handle<GameObject> gameObject,const reflect::dynamic& component) 
		: component_name{}
	{
		game_object_handle = gameObject;
		component_reflect.swap(component.copy());
	}

	bool CMD_AddComponent::execute()
	{
		if (game_object_handle) {
			if (!component_name.empty())
				new_component_handle = game_object_handle->AddComponent(reflect::get_type(component_name));
			//Find all Commands of similar objects in the controller and modify the handle to point to this!
			else {
				new_component_handle = game_object_handle->AddComponent(component_reflect);
			}
			return true;
		}
		return false;
	}

	bool CMD_AddComponent::undo()
	{
		if (game_object_handle) {
			if (new_component_handle) {
				game_object_handle->RemoveComponent(new_component_handle);
				return true;
			}
			else if (game_object_handle->GetComponent(reflect::get_type(component_name))) { //This is when the gameObjectHandle has a different component handle due to DeleteGameObjectCMD
				game_object_handle->RemoveComponent(game_object_handle->GetComponent(reflect::get_type(component_name)));
				return true;
			}
			else {
				return false;
			}
		}

		return false;
	}

}
