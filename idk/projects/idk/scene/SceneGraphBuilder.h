#pragma once
#include <ds/span.h>
#include <ds/slow_tree.h>
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

	private:
		hash_table<
			Handle<class GameObject>, 
			SceneGraph*
		> sg_lookup;

		SceneGraph scene_graphs;
	};

}
