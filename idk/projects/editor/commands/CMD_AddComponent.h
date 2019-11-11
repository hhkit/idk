//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_AddComponent.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <editor/commands/ICommand.h>
#include <core/GameObject.h>

namespace idk {

	class CMD_AddComponent : public ICommand { //serialize/deserialize use text.h
	public:
		CMD_AddComponent(Handle<GameObject> gameObject, string component);
		CMD_AddComponent(Handle<GameObject> gameObject, const reflect::dynamic& component);

		virtual bool execute() override;

		virtual bool undo() override;

	protected:

		GenericHandle		new_component_handle	{};
		const string		component_name			{};
		reflect::dynamic	component_reflect		{};
	};

}