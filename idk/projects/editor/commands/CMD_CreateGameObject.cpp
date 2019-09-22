//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_CreateGameObject.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_CreateGameObject.h>
#include <common/Transform.h>
#include <scene/SceneManager.h>

namespace idk {


	CMD_CreateGameObject::CMD_CreateGameObject(Handle<GameObject> go)
	{
		parenting_gameobject = go;
	}

	CMD_CreateGameObject::CMD_CreateGameObject()
	{
	}

	bool CMD_CreateGameObject::execute()
	{
		game_object_handle = Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject();

		if (parenting_gameobject) {
			game_object_handle->GetComponent<Transform>()->SetParent(parenting_gameobject,false);
		}

		return game_object_handle ? true : false; //Return true if create gameobject is successful
	}

	bool CMD_CreateGameObject::undo()
	{
		if (game_object_handle) {
			Core::GetSystem<SceneManager>().GetActiveScene()->DestroyGameObject(game_object_handle);
			game_object_handle = {}; //Reassign to blank
			return true;
		}
		else {
			return false;
		}
	}

}
