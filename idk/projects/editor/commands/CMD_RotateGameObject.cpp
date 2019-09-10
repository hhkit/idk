//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_RotateGameObject.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <common/Transform.h>
#include <editor/commands/CMD_RotateGameObject.h>

namespace idk {

	CMD_RotateGameObject::CMD_RotateGameObject(Handle<GameObject> gameObject, const quat& rot)
		: rotation{ rot } 
	{
		game_object_handle = gameObject;
	}

	bool CMD_RotateGameObject::execute()
	{
		//Do rotations
		return false;
	}

	bool CMD_RotateGameObject::undo()
	{
		//Undo rotations
		return false;
	}

}
