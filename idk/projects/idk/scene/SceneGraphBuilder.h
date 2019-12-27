#pragma once
#include <ds/span.h>
#include <ds/slow_tree.inl>
#include <idk.h>
#include <core/Core.h>

namespace idk
{
	class SceneGraphBuilder
	{
	public:
		using SceneGraph = slow_tree<Handle<class GameObject>>;

		void BuildSceneGraph(span<const GameObject> objs);
		SceneGraph& FetchSceneGraph();
		SceneGraph* FetchSceneGraphFor(Handle<class GameObject>);
		void ReparentObject(Handle<class GameObject> go, Handle<class GameObject> new_parent);
		void InsertObject(Handle<class GameObject> go);
	private:
		hash_table<
			Handle<class GameObject>, 
			SceneGraph*
		> sg_lookup;

		SceneGraph scene_graphs;
		size_t slot{};
		bool dirty = true;
	};

}
