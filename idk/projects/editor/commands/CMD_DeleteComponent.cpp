//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_DeleteComponent.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		26 OCT 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_DeleteComponent.h>
#include <iostream>
namespace idk {


	CMD_DeleteComponent::CMD_DeleteComponent(Handle<GameObject> gameObject, GenericHandle component)
		:component_name{ string((*component).type.name()) }
	{
		serialized_component.first = component;
		serialized_component.second.swap((*serialized_component.first).copy());
		game_object_handle = gameObject;
	}

	CMD_DeleteComponent::CMD_DeleteComponent(Handle<GameObject> gameObject, string Name)
		:component_name { Name }
	{

		serialized_component.first = game_object_handle->GetComponent(reflect::get_type(component_name));
		serialized_component.second.swap((*serialized_component.first).copy());
		game_object_handle = gameObject;
	}

	bool CMD_DeleteComponent::execute()
	{

		if (game_object_handle) {
				
			std::cout << "Removing Component with ID: " << serialized_component.first.id << std::endl;
			game_object_handle->RemoveComponent(serialized_component.first);

			return true;
		}
		return false;


	}

	bool CMD_DeleteComponent::undo()
	{
		if (game_object_handle) {
			auto newComponentHandle = game_object_handle->AddComponent(serialized_component.second);
			serialized_component.first = newComponentHandle;
			std::cout << "Adding Component with ID: " << newComponentHandle.id << std::endl;

			return true;
		}

		return false;
	}

}
