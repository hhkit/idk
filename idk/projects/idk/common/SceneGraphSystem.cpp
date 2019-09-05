#include "stdafx.h"
#include "SceneGraphSystem.h"
#include <core/GameObject.h>

namespace idk
{
	void SceneGraphSystem::Init()
	{
	}

	void SceneGraphSystem::Shutdown()
	{
	}

	void SceneGraphSystem::BuildSceneGraph(span<const GameObject> objs)
	{
		sg_lookup.clear(); // throw away old scene graph

		// rebuild

		// defrag??
		for (auto& elem : objs)
		{
		//	sg_lookup.find();
		}
	}

	SceneGraphSystem::SceneGraph* SceneGraphSystem::FetchSceneGraph(Handle<class GameObject> findme)
	{
		auto itr = sg_lookup.find(findme);
		if (itr == sg_lookup.end())
			return nullptr;
		return itr->second;
	}
}