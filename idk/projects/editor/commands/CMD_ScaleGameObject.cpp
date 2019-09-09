//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_ScaleGameObject.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		30 AUG 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_ScaleGameObject.h>
#include <common/Transform.h>

namespace idk {
	CMD_ScaleGameObject::CMD_ScaleGameObject(Handle<GameObject> gameObject, const vec3& sca)
		: game_object_handle{ gameObject }, scale{ sca } 
	{}

	bool CMD_ScaleGameObject::execute()
	{
		if (game_object_handle) {
			Handle<Transform> transformHandle = game_object_handle->GetComponent<Transform>();
			if (transformHandle) {
				transformHandle->scale += scale;
				return true;
			}
			else {
				return false;
			}
		}
		return false;
	}
	bool CMD_ScaleGameObject::undo()
	{
		if (game_object_handle) {
			Handle<Transform> transformHandle = game_object_handle->GetComponent<Transform>();
			if (transformHandle) {
				transformHandle->scale -= scale;
				return true;
			}
			else {
				return false;
			}
		}
		return false;
	}
}
