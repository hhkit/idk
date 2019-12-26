//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_MoveGameObject.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	Moves gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_TransformGameObject.h>
#include <common/Transform.h>

namespace idk {

	CMD_TransformGameObject::CMD_TransformGameObject(Handle<GameObject> gameObject, const mat4& matrix_old, const mat4& matrix_new)
		: original_values{ matrix_old }, new_values{ matrix_new }
	{
		game_object_handle = gameObject;
	}

	bool CMD_TransformGameObject::execute() {
		//Skips first execute
		if (!first_execute) {
			first_execute = true;
			if (game_object_handle)
				return true;
			else
				return false;
		}

		if (game_object_handle) 
		{
			const Handle<Transform> transformHandle = game_object_handle->Transform();
			transformHandle->GlobalMatrix(new_values);
		}
		return false;
	}

	bool CMD_TransformGameObject::undo()  {
		if (game_object_handle) 
		{
			const Handle<Transform> transformHandle = game_object_handle->Transform();
			transformHandle->GlobalMatrix(original_values);
		}
		return false;
	}


}
