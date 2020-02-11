#pragma once
#include "SceneGraph.h"

namespace idk
{
	template<typename Visitor>
	inline void SceneGraphHandle::Visit(Visitor&& visitor) const
	{
		if (_scene_graph)
			_scene_graph->Visit(std::forward<Visitor>(visitor), _root);
	}

	template<typename Visitor>
	inline void SceneGraph::Visit(Visitor&& visitor, Handle<GameObject> root) const
	{
		int depth = 0;
		int last_depth = 0;

		if (root == Handle<GameObject>{})
		{
			for (GenericHandle::scene_t scn = 0; scn < MaxScene; ++scn)
			{
				if (_per_scene->nodes.empty())
					continue;

				++depth;
				for (GenericHandle::index_t index = 0; index < _per_scene[scn].nodes.size(); ++index)
					VisitImpl(std::forward<Visitor>(visitor), Handle<GameObject>{index, _per_scene[scn].gens[index], scn}, depth, last_depth);
				--depth;
			}
			return;
		}

		VisitImpl(std::forward<Visitor>(visitor), root, depth, last_depth);
	}

	template<typename Visitor>
	inline void SceneGraph::VisitImpl(Visitor&& visitor, Handle<GameObject> root, int& depth, int& last_depth) const
	{
		int depth_change = depth - last_depth;
		last_depth = depth;

		bool recurse = true;
		if constexpr (std::is_same_v<std::decay_t<decltype(visitor(root, depth_change))>, void>)
			visitor(root, depth_change);
		else
		{
			static_assert(std::is_same_v<std::decay_t<decltype(visitor(root, depth_change))>, bool>, "visitor of tree must be either void or bool");
			recurse = visitor(root, depth_change);
		}

		if (recurse)
		{
			++depth;
			for (auto index : _per_scene[root.scene].nodes[root.index])
				VisitImpl(std::forward<Visitor>(visitor), Handle<GameObject>{index, _per_scene[root.scene].gens[index], root.scene}, depth, last_depth);
			--depth;
		}
	}
}