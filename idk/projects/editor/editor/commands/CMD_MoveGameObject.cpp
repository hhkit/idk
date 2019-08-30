//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_MoveGameObject.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		30 AUG 2019
//@brief	Moves gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_MoveGameObject.h>
#include <common/Transform.h>

namespace idk {

	CMD_MoveGameObject::CMD_MoveGameObject(Handle<GameObject> gameObject, const vec3& pos) 
		:game_object_handle{ gameObject }, position{ pos } {}

	bool CMD_MoveGameObject::execute() {
		if (game_object_handle) {
			Handle<Transform> transformHandle = game_object_handle->GetComponent<Transform>();
			if (transformHandle) {
				transformHandle->position += position;
				return true;
			}
			else {
				return false;
			}
		}
		return false;
	}

	bool CMD_MoveGameObject::undo()  {
		if (game_object_handle) {
			Handle<Transform> transformHandle = game_object_handle->GetComponent<Transform>();
			if (transformHandle) {
				transformHandle->position -= position;
				return true;
			}
			else {
				return false;
			}
		}
		return false;
	}


}
