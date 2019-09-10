//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_DeleteGameObject.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	Delete gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_DeleteGameObject.h>
#include <scene/SceneManager.h>
#include <common/Name.h>
#include <common/Transform.h>
#include <serialize/serialize.h> //Serialize/Deserialize

namespace idk {



	CMD_DeleteGameObject::CMD_DeleteGameObject(Handle<GameObject> gameObject)
	{
		game_object_handle = gameObject;
		//Copy all components from this gameobject
		for (auto& c : gameObject->GetComponents())
			vector_of_components.emplace_back((*c).copy());


	}

	bool CMD_DeleteGameObject::execute()
	{
		if (game_object_handle) {
			//Serialize before deleting!
			Core::GetSystem<SceneManager>().GetActiveScene()->DestroyGameObject(game_object_handle);
			//Find and get all CMDs using Handle<GameObject> TODO

			return true;
		}
		return false;
	}

	bool CMD_DeleteGameObject::undo()
	{
		game_object_handle = Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject();

		if (game_object_handle) {
			for (auto& c : vector_of_components) {

				if (c.is<Transform>())
				{
					Transform& t = c.get<Transform>();
					game_object_handle->GetComponent<Transform>()->position = t.position;
					game_object_handle->GetComponent<Transform>()->rotation = t.rotation;
					game_object_handle->GetComponent<Transform>()->scale	= t.scale;
					game_object_handle->GetComponent<Transform>()->parent	= t.parent;
				}
				else if (c.is<Name>())
				{
					Name& t = c.get<Name>();
					game_object_handle->GetComponent<Name>()->name = t.name;
				}
				else {
					game_object_handle->AddComponent(c);
				}

				//Reassign all CMDs using Handle<GameObject> TODO

			}
			return true;
		}
		else {
			return false;
		}
	}

}
