#pragma once
#include <ds/span.h>
#include <scene/SceneGraph.h>
#include <idk.h>
#include <core/Core.h>

namespace idk
{
	class SceneGraphBuilder
	{
	public:
		void BuildSceneGraph(span<const GameObject> objs);
		SceneGraphHandle FetchSceneGraph();
		SceneGraphHandle FetchSceneGraphFor(Handle<class GameObject>);
		void ReparentObject(Handle<class GameObject> go, Handle<class GameObject> new_parent);
		void InsertObject(Handle<class GameObject> go);

	private:
		SceneGraph _scene_graph;
		bool _dirty = true;
	};

}
