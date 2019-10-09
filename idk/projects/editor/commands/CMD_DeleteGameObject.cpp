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
#include <IDE.h>				 //IDE

namespace idk {



	CMD_DeleteGameObject::CMD_DeleteGameObject(Handle<GameObject> gameObject)
	{
		game_object_handle = gameObject;
		

	}

	bool CMD_DeleteGameObject::execute()
	{
		if (game_object_handle) {
			
			IDE& editor = Core::GetSystem<IDE>();

			//Find and get all CMDs using Handle<GameObject>
			commands_affected.clear();
			for (const auto& i : editor.command_controller.undoStack) {
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

			gameobject_vector.clear();
			editor.RecursiveCollectObjects(game_object_handle,gameobject_vector);

			Core::GetSystem<SceneManager>().GetActiveScene()->DestroyGameObject(game_object_handle);

			return true;
		}
		return false;
	}

	bool CMD_DeleteGameObject::undo()
	{

		try {
			RecursiveCreateObjects(gameobject_vector, true);
		}
		catch (const bool fail) {
			return fail;
		}

		//Reassign all CMDs using Handle<GameObject> Big O(n^2)
		IDE& editor = Core::GetSystem<IDE>();
		for (const auto& i : commands_affected) {
			for (const auto& j : editor.command_controller.undoStack) {
				if (i == j.get()) { //If this unique pointer matches with the commands_affected
					j->game_object_handle = game_object_handle;
					break;
				}
			}
		}

		return true;
	}

	void CMD_DeleteGameObject::RecursiveCreateObjects(vector<RecursiveObjects>& vector_ref, bool isRoot)
	{

		for (RecursiveObjects& object : vector_ref) {
			Handle<GameObject> i = Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject();
			if (isRoot) {
				game_object_handle = i;
				isRoot = false;
			}

			if (!game_object_handle) {
				throw false; //Throws if the recursion fails
			}

			for (auto& c : object.vector_of_components) {

				if (c.is<Transform>())
				{
					Transform& t = c.get<Transform>();
					i->GetComponent<Transform>()->position = t.position;
					i->GetComponent<Transform>()->rotation = t.rotation;
					i->GetComponent<Transform>()->scale = t.scale;
					if (object.parent_of_children) {
						i->GetComponent<Transform>()->parent = object.parent_of_children;
					}
					else {
						i->GetComponent<Transform>()->parent = t.parent;
					}
				}
				else if (c.is<Name>())
				{
					Name& t = c.get<Name>();
					i->GetComponent<Name>()->name = t.name;
				}
				else {
					i->AddComponent(c);
				}
			}

			//Assign children with this gameobject as parent
			for (auto& childObject : object.children) {
				childObject.parent_of_children = i;
			}

			RecursiveCreateObjects(object.children);
		}
	}

}
