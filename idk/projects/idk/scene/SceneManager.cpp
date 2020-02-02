#include "stdafx.h"
#include "SceneManager.h"
#include <core/Core.h>
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <res/MetaBundle.inl>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <scene/SceneFactory.h>
#include <serialize/text.h>
#include <util/ioutils.h>
#include <file/FileSystem.h>
#include <core/GameState.inl>

#include <res/Guid.inl>
#include <iostream>
#include <ds/span.inl>

namespace idk
{
	void SceneManager::Init()
	{
		Core::GetResourceManager().RegisterFactory<SceneFactory>();
		Core::GetResourceManager().RegisterLoader<SceneLoader>(Scene::ext);
	}

	void SceneManager::LateInit()
	{
		for (auto& elem : Core::GetSystem<FileSystem>().GetFilesWithExtension("/assets", Scene::ext, FS_FILTERS::ALL))
			Core::GetResourceManager().Load(elem);
		
		_prefab_scene = Core::GetResourceManager().LoaderEmplaceResource<Scene>(Scene::prefab);
		_prefab_scene->Activate();
	}

	void SceneManager::EarlyShutdown()
	{
		for (auto& elem : GameState::GetGameState().GetObjectsOfType<GameObject>())
			GameState::GetGameState().DestroyObject(elem.GetHandle());

		GameState::GetGameState().DestroyQueue();
	}

	void SceneManager::Shutdown()
	{
	}

	RscHandle<Scene> SceneManager::StartupScene() const
	{
		return _startup_scene;
	}

	bool SceneManager::StartupScene(RscHandle<Scene> scene)
	{
		if (scene)
		{
			_startup_scene = scene;
			return true;
		}
		return false;
	}

	RscHandle<Scene> SceneManager::GetSceneByBuildIndex(unsigned char index) const
	{
		if (index == Scene::prefab)
			return _prefab_scene;

		return _scenes[index];
	}

	RscHandle<Scene> SceneManager::GetActiveScene() const
	{
		return _active_scene;
	}

	RscHandle<Scene> SceneManager::GetPrefabScene() const
	{
		return _prefab_scene;
	}

	bool SceneManager::SetActiveScene(RscHandle<Scene> s)
	{
		_active_scene = s;
		OnSceneChange.Fire(_active_scene);
		return true;
	}

	void SceneManager::SetNextScene(RscHandle<Scene> s)
	{
		changing = true;
		_next_scene = s;
	}

	void SceneManager::ChangeScene()
	{
		if (changing)
		{
			_active_scene->Deactivate();
			_active_scene = _next_scene;
			_active_scene->LoadFromResourcePath();
			BuildSceneGraph(Core::GetGameState().GetObjectsOfType<const GameObject>());
			OnSceneChange.Fire(_active_scene);
			changing = false;
		}
	}

	void SceneManager::DestroyQueuedObjects(span<GameObject> objs)
	{
		GameState::GetGameState().SortObjectsOfType<GameObject>(
			[](const auto& lhs, const auto& rhs)
			{
				return lhs.Transform()->Depth() == rhs.Transform()->Depth()
					? lhs.GetHandle().id < rhs.GetHandle().id
					: lhs.Transform()->Depth() < rhs.Transform()->Depth();
			}
		);

		for (auto& elem : objs)
		{
			if (elem.HierarchyIsQueuedForDestruction())
				GameState::GetGameState().DestroyObject(elem.GetHandle());
		}
		GameState::GetGameState().DestroyQueue();
		GameState::GetGameState().FlushCreationQueue();
	}

	void SceneManager::BuildSceneGraph(span<const GameObject> objs)
	{
		_sg_builder.BuildSceneGraph(objs);
	}

	SceneManager::SceneGraph& SceneManager::FetchSceneGraph()
	{
		return _sg_builder.FetchSceneGraph();
	}

	SceneManager::SceneGraph* SceneManager::FetchSceneGraphFor(Handle<class GameObject> handle)
	{
		return _sg_builder.FetchSceneGraphFor(handle);
	}

	void SceneManager::ReparentObject(Handle<class GameObject> go, Handle<class GameObject> new_parent)
	{
		_sg_builder.ReparentObject(go, new_parent);
	}

	void SceneManager::InsertObject(Handle<class GameObject> go)
	{
		_sg_builder.InsertObject(go);
	}

}