//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_CreateGameObject.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		9 Sept 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_CreateGameObject.h>
#include <scene/SceneManager.h>

namespace idk {


	CMD_CreateGameObject::CMD_CreateGameObject()
	{
	}

	bool CMD_CreateGameObject::execute()
	{
		new_game_object_handle = Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject();
		return new_game_object_handle ? true : false; //Return true if create gameobject is successful
	}

	bool CMD_CreateGameObject::undo()
	{
		if (new_game_object_handle) {
			Core::GetSystem<SceneManager>().GetActiveScene()->DestroyGameObject(new_game_object_handle);
			return true;
		}
		else {
			return false;
		}
	}

}
