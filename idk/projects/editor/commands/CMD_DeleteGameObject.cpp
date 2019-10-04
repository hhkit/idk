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
#include <iostream>		//IDE

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
			RecursiveCollectObjects(game_object_handle,gameobject_vector);

			Core::GetSystem<SceneManager>().GetActiveScene()->DestroyGameObject(game_object_handle);

			return true;
		}
		return false;
	}

	bool CMD_DeleteGameObject::undo()
	{
		//game_object_handle = Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject();
		
		//if (game_object_handle) {
		try {
			RecursiveCreateObjects(gameobject_vector, true);
		}
		catch (bool) {
			return false;
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
		//}
		//else {
		//	return false;
		//}
	}

	void CMD_DeleteGameObject::RecursiveCollectObjects(Handle<GameObject> i, vector<RecursiveObjects>& vector_ref)
	{
		
		RecursiveObjects newObject{};
		//Copy all components from this gameobject
		for (auto& c : i->GetComponents())
			newObject.vector_of_components.emplace_back((*c).copy());

		SceneManager& sceneManager = Core::GetSystem<SceneManager>();
		SceneManager::SceneGraph* children = sceneManager.FetchSceneGraphFor(i);

		if (children) {
			//std::cout << "NUM OF CHILDREN: " << children->size() << std::endl;

			//If there is no children, it will stop
			children->visit([&](const Handle<GameObject>& handle, int depth) -> bool { //Recurse through one level only
				(void)depth;

				//Skip parent
				//std::cout << "CHILD: " << handle->GetComponent<Name>()->name << std::endl;
				if (handle == i)
					return true;

				RecursiveCollectObjects(handle, newObject.children); //Depth first recursive

				return false;

				});
		}
		vector_ref.push_back(newObject);
	}

	void CMD_DeleteGameObject::RecursiveCreateObjects(vector<RecursiveObjects>& vector_ref, bool isRoot)
	{
		//if (!i) {
		//	i = Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject();
		//
		//}
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
					i->GetComponent<Transform>()->position	= t.position;
					i->GetComponent<Transform>()->rotation	= t.rotation;
					i->GetComponent<Transform>()->scale		= t.scale;
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
