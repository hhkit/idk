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
#include <IDE.h>		//IDE

namespace idk {



	CMD_DeleteGameObject::CMD_DeleteGameObject(Handle<GameObject> gameObject)
	{
		game_object_handle = gameObject;
		//Copy all components from this gameobject
		//for (auto& c : gameObject->GetComponents())
		//	vector_of_components.emplace_back((*c).copy());


	}

	bool CMD_DeleteGameObject::execute()
	{
		if (game_object_handle) {
			//Serialize before deleting!
			IDE& editor = Core::GetSystem<IDE>();
			auto iterator = editor.selected_gameObjects.begin();
			for (; iterator != editor.selected_gameObjects.end(); ++iterator) {
				if (*iterator == game_object_handle) {
					break;
				}
			}

			if (iterator != editor.selected_gameObjects.end())
				editor.selected_gameObjects.erase(iterator);

			serialized_gameobject = serialize_text(game_object_handle);

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
			parse_text(serialized_gameobject, game_object_handle);
			return true;
		}
		else {
			return false;
		}
	}

}
