#include "stdafx.h"
#include "SceneGraphBuilder.h"
#include <core/GameObject.h>
#include <core/GameState.inl>
#include <common/Transform.h>

namespace idk
{
	void SceneGraphBuilder::BuildSceneGraph(span<const GameObject> objs)
	{
		sg_lookup.clear(); // throw away old scene graphs
		scene_graphs.clear();
		// defrag by depth
		GameState::GetGameState().SortObjectsOfType<GameObject>(
			[](const auto& lhs, const auto& rhs)
			{
				const auto ldepth = lhs.Transform()->Depth();
				const auto rdepth = rhs.Transform()->Depth();
				return ldepth == rdepth
					?  lhs.GetHandle().id < rhs.GetHandle().id
					:  ldepth < rdepth;
			}
		);

		// we now guarantee that all parents are before their children

		// rebuild scene graph
		for (auto& elem : objs)
		{
			/*/
			InsertObject(elem.GetHandle());
			/*/
			auto key = elem.GetHandle();
			const auto parent = elem.Parent();
			assert(key);
			if (parent)
			{
				auto& sg = sg_lookup[parent]->emplace_child(key);
				sg_lookup.try_emplace(key, &sg);
			}
			else
				sg_lookup.try_emplace(key, &scene_graphs.emplace_child(key));
			//*/
		}

		// we now have the rebuilt scene graph
	}

	SceneGraphBuilder::SceneGraph& SceneGraphBuilder::FetchSceneGraph()
	{
		return scene_graphs;
	}

	SceneGraphBuilder::SceneGraph* SceneGraphBuilder::FetchSceneGraphFor(Handle<class GameObject> findme)
	{
		auto itr = sg_lookup.find(findme);
		if (itr == sg_lookup.end())
			return nullptr;
		return itr->second;
	}

	void SceneGraphBuilder::ReparentObject(Handle<class GameObject> go, Handle<class GameObject> new_parent)
	{
		auto curr_parent = go->Parent();

		// remove from current parent
		if (curr_parent)
		{
			const auto itr = sg_lookup.find(curr_parent);
			IDK_ASSERT(itr != sg_lookup.end());
			itr->second->pop_child(go);
		}
		else
		{
			scene_graphs.pop_child(go);
		}

		if (new_parent)
		{
			const auto itr = sg_lookup.find(new_parent);
			IDK_ASSERT(itr != sg_lookup.end());
			sg_lookup[go] = &itr->second->emplace_child(go);
			
		}
		else
		{
			sg_lookup[go] = &scene_graphs.emplace_child(go);
		}
	}

	void SceneGraphBuilder::InsertObject(Handle<class GameObject> go)
	{
		auto itr = sg_lookup.find(go);
		if (itr == sg_lookup.end())
		{
			auto parent = go->Parent();
			if (parent)
			{
				auto parent_sg = sg_lookup.find(parent);
				if (parent_sg == sg_lookup.end())
					InsertObject(parent);
				sg_lookup[go] = &sg_lookup[parent]->emplace_child(go);
			}
			else
			{
				sg_lookup[go] = &scene_graphs.emplace_child(go);
			}
		}
	}

}