#include "stdafx.h"
#include "SceneGraphBuilder.h"
#include <core/GameObject.inl>
#include <core/GameState.inl>
#include <common/Transform.h>
#include <ds/span.inl>

namespace idk
{
	void SceneGraphBuilder::BuildSceneGraph(span<const GameObject> objs)
	{
		_scene_graph.Build(objs);
	}

	SceneGraphHandle SceneGraphBuilder::FetchSceneGraph()
	{
		return _scene_graph.GetHandle();
	}

	SceneGraphHandle SceneGraphBuilder::FetchSceneGraphFor(Handle<class GameObject> findme)
	{
		return _scene_graph.GetHandle(findme);
	}

	void SceneGraphBuilder::ReparentObject(Handle<class GameObject> go, Handle<class GameObject> new_parent)
	{
		// temporary
		_scene_graph.Build(Core::GetGameState().GetObjectsOfType<GameObject>());
	}

	void SceneGraphBuilder::InsertObject(Handle<class GameObject> go)
	{
		// temporary
		_scene_graph.Build(Core::GetGameState().GetObjectsOfType<GameObject>());
	}

}