//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_DeleteGameObject.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		02 OCT 2019
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
		for (auto& c : gameObject->GetComponents())
			vector_of_components.emplace_back((*c).copy());

	}

	bool CMD_DeleteGameObject::execute()
	{
		if (game_object_handle) {
			
			IDE& editor = Core::GetSystem<IDE>();

			//Find and get all CMDs using Handle<GameObject>
			commands_affected.clear();
			for (unique_ptr<ICommand>& i : editor.command_controller.undoStack) {
				if (i->game_object_handle == game_object_handle) {
					commands_affected.push_back(i.get());
				}
			}
			//Find and deselect from selected gameobject
			auto iterator = editor.selected_gameObjects.begin();
			for (; iterator != editor.selected_gameObjects.end(); ++iterator) {
				if (*iterator == game_object_handle) {
					break;
				}
			}

			if (iterator != editor.selected_gameObjects.end())
				editor.selected_gameObjects.erase(iterator);

			Core::GetSystem<SceneManager>().GetActiveScene()->DestroyGameObject(game_object_handle);


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
				
			}

			//Reassign all CMDs using Handle<GameObject> Big O(n^2)
			IDE& editor = Core::GetSystem<IDE>();
			for (ICommand* i : commands_affected) {
				for (unique_ptr<ICommand>& j : editor.command_controller.undoStack) {
					if (i == j.get()) { //If this unique pointer matches with the commands_affected
						j->game_object_handle = game_object_handle;
						break;
					}
				}
			}

			return true;
		}
		else {
			return false;
		}
	}

}
