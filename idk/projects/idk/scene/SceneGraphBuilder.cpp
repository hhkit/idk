#include "stdafx.h"
#include "SceneGraphBuilder.h"
#include <core/GameObject.h>
#include <core/GameState.h>
#include <common/Transform.h>

namespace idk
{
	void SceneGraphBuilder::BuildSceneGraph(span<const GameObject> objs)
	{
		sg_lookup.clear(); // throw away old scene graph
		// defrag by depth
		GameState::GetGameState().SortObjectsOfType<GameObject>(
			[](const auto& lhs, const auto& rhs)
			{
				return lhs->Transform()->Depth() < rhs->Transform()->Depth();
			}
		);

		// we now guarantee that all parents are before their children

		// rebuild scene graph
		for (auto& elem : objs)
		{
			auto key = elem.GetHandle();
			auto parent = elem.Parent();
			if (parent)
			{
				auto& sg = sg_lookup[parent]->emplace_child(key);
				sg_lookup.try_emplace(key, &sg);
			}
			else
				sg_lookup.try_emplace(key, &scene_graphs.emplace_child(key));
		}

		// we now have the rebuilt scene graph
	}

	SceneGraphBuilder::SceneGraph* SceneGraphBuilder::FetchSceneGraph(Handle<class GameObject> findme)
	{
		auto itr = sg_lookup.find(findme);
		if (itr == sg_lookup.end())
			return nullptr;
		return itr->second;
	}
}