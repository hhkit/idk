//////////////////////////////////////////////////////////////////////////////////
//@file		CMD_DeleteGameObject.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		02 OCT 2019
//@brief	Delete gameobjects
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "CMD_DeleteGameObject.h"
#include <scene/SceneManager.h>
#include <common/Name.h>
#include <common/Transform.h>
#include <serialize/text.h>
#include <IDE.h>
#include <res/ResourceHandle.inl>

namespace idk 
{
	CMD_DeleteGameObject::CMD_DeleteGameObject(Handle<GameObject> gameObject)
	{
		game_object_handle = gameObject;
	}

	bool CMD_DeleteGameObject::execute()
	{
		if (game_object_handle) 
		{
			IDE& editor = Core::GetSystem<IDE>();
			if (gameobject_vector.empty())
				editor.RecursiveCollectObjects(game_object_handle, gameobject_vector);
			Core::GetSystem<SceneManager>().GetActiveScene()->DestroyGameObject(game_object_handle);
			return true;
		}
		return false;
	}

	bool CMD_DeleteGameObject::undo()
	{
		return RecursiveCreateObjects(gameobject_vector, true);
	}

	bool CMD_DeleteGameObject::RecursiveCreateObjects(vector<RecursiveObjects>& vector_ref, bool isRoot)
	{
		for (RecursiveObjects& object : vector_ref) 
		{
			Handle<GameObject> i = Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject(object.original_handle);
			if (isRoot) {
				game_object_handle = i;
				isRoot = false;
			}

			if (!game_object_handle)
				return false; // Return false if the recursion fails

			for (auto& c : object.vector_of_components) 
			{
				if (c.is<Transform>())
				{
					Transform& t = c.get<Transform>();
					i->GetComponent<Transform>()->position = t.position;
					i->GetComponent<Transform>()->rotation = t.rotation;
					i->GetComponent<Transform>()->scale = t.scale;
					if (object.parent_of_children)
						i->GetComponent<Transform>()->parent = object.parent_of_children;
					else
						i->GetComponent<Transform>()->parent = t.parent;
				}
				else if (c.is<Name>())
				{
					Name& t = c.get<Name>();
					i->GetComponent<Name>()->name = t.name;
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
