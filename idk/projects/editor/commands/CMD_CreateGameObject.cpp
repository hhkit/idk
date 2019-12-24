//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_CreateGameObject.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		10 SEPT 2019
//@brief	Adds components to gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/commands/CMD_CreateGameObject.h>
#include <editor/windows/IGE_HierarchyWindow.h>
#include <common/Transform.h>
#include <common/Name.h>
#include <scene/SceneManager.h>
#include <prefab/PrefabInstance.h>
#include <IDE.h>				 //IDE
#include <reflect/reflect.inl>
#include <res/ResourceHandle.inl>

namespace idk {


	CMD_CreateGameObject::CMD_CreateGameObject(Handle<GameObject> go, string name, vector<string> initial_components)
        : parenting_gameobject{ go }, name{ name }, initial_components{ initial_components }
	{
	}

	CMD_CreateGameObject::CMD_CreateGameObject(vector<RecursiveObjects> recursiveObj) : copied_object{recursiveObj}
	{
	}

	bool CMD_CreateGameObject::execute()
	{
		if (copied_object.empty()) {
			game_object_handle = Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject();

			if (parenting_gameobject) {
				game_object_handle->GetComponent<Transform>()->SetParent(parenting_gameobject, false);
			}
			if (name.size())
				game_object_handle->Name(name);
			for (const auto& str : initial_components)
				game_object_handle->AddComponent(string_view{ str });
			Core::GetSystem<IDE>().selected_gameObjects.clear();
			Core::GetSystem<IDE>().selected_gameObjects.push_back(game_object_handle);
			Core::GetSystem<IDE>().FindWindow< IGE_HierarchyWindow>()->ScrollToSelectedInHierarchy(game_object_handle);
			Core::GetSystem<IDE>().RefreshSelectedMatrix();
			return bool(game_object_handle); //Return true if create gameobject is successful
		}
		else {
			try {
				RecursiveCreateObjects(copied_object, true);
			}
			catch (const bool fail) {
				return fail;
			}
			Core::GetSystem<IDE>().selected_gameObjects.clear();
			Core::GetSystem<IDE>().selected_gameObjects.push_back(game_object_handle);
			Core::GetSystem<IDE>().FindWindow< IGE_HierarchyWindow>()->ScrollToSelectedInHierarchy(game_object_handle);
			Core::GetSystem<IDE>().RefreshSelectedMatrix();
			return true;
		}
	}
	
	bool CMD_CreateGameObject::undo()
	{
		if (game_object_handle) {

			IDE& editor = Core::GetSystem<IDE>();
		
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

	void CMD_CreateGameObject::RecursiveCreateObjects(vector<RecursiveObjects>& vector_ref, bool isRoot)
	{
        static bool copy_as_prefab = false;
        if (isRoot)
        {
            copy_as_prefab = false;
            for (const auto& c : vector_ref[0].vector_of_components)
            {
                if (c.is<PrefabInstance>())
                {
                    if (c.get<PrefabInstance>().object_index == 0)
                        copy_as_prefab = true;
                    break;
                }
            }
        }

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
                else if (c.is<PrefabInstance>())
                {
                    if (copy_as_prefab)
                        i->AddComponent(c);
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
