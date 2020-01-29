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
		if (copied_object.empty()) // create new game object, not a copy
		{
			if (game_object_handle == Handle<GameObject>{})
				game_object_handle = Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject();
			else
				Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject(game_object_handle);

			if (parenting_gameobject) {
				game_object_handle->GetComponent<Transform>()->SetParent(parenting_gameobject, false);
			}
			if (name.size())
				game_object_handle->Name(name);
			for (const auto& str : initial_components)
				game_object_handle->AddComponent(string_view{ str });

			return bool(game_object_handle); //Return true if create gameobject is successful
		}
		else
		{
			if (!RecursiveCreateObjects(copied_object, true))
				return false;
			return true;
		}
	}
	
	bool CMD_CreateGameObject::undo()
	{
		if (!game_object_handle)
			return false;
		Core::GetSystem<SceneManager>().GetActiveScene()->DestroyGameObject(game_object_handle);
		return true;
	}

	bool CMD_CreateGameObject::RecursiveCreateObjects(vector<RecursiveObjects>& vector_ref, bool isRoot)
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

		for (RecursiveObjects& object : vector_ref) 
		{
			if (object.preserved_handle == Handle<GameObject>{})
				object.preserved_handle = Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject();
			else
				Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject(object.preserved_handle);

			auto i = object.preserved_handle;
			if (isRoot) 
			{
				game_object_handle = i;
				isRoot = false;
			}

			if (!game_object_handle)
				return false; //Throws if the recursion fails

			for (auto& c : object.vector_of_components)
			{
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
				else
					i->AddComponent(c);
			}

			//Assign children with this gameobject as parent
			for (auto& childObject : object.children)
				childObject.parent_of_children = i;

			if (!RecursiveCreateObjects(object.children))
				return false;
		}

		return true;
	}

}
