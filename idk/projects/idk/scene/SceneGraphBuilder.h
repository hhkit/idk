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
		using SceneGraph = slow_tree<Handle<class Gameobject>>;

		void BuildSceneGraph(span<const GameObject> objs);
		SceneGraph* FetchSceneGraph(Handle<class GameObject>);

	private:
		hash_table<
			Handle<class GameObject>, 
			SceneGraph*
		> sg_lookup;

		slow_tree<Handle<class GameObject>> scene_graphs;
	};

}
