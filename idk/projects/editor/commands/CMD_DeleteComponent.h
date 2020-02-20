//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_DeleteComponent.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		26 OCT 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <editor/commands/ICommand.h>
#include <core/GameObject.h>
#include <prefab/PrefabInstance.h>

namespace idk 
{

	class CMD_DeleteComponent : public ICommand 
	{
	public:
		CMD_DeleteComponent(Handle<GameObject> gameObject, string componentName);
		CMD_DeleteComponent(Handle<GameObject> gameObject, GenericHandle componentToRemove); // Only if specific Handle is specified

		virtual bool execute() override;
		virtual bool undo() override;

	private:
		GenericHandle component;
		string_view component_name;
		reflect::dynamic component_data;
		int prefab_component_index = -1;

	};

}