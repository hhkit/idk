#include "stdafx.h"
#include "SceneManager.h"
#include <core/Core.h>
#include <core/GameObject.h>
#include <common/Transform.h>
#include <res/MetaBundle.h>
#include <scene/SceneFactory.h>
#include <serialize/text.h>
#include <util/ioutils.h>

#include <iostream>

namespace idk
{
	RscHandle<Scene> SceneManager::StartupScene() const
	{
		return _startup_scene;
	}

	bool SceneManager::StartupScene(RscHandle<Scene> scene)
	{
		for (auto& elem : _scenes)
		{
			if (elem.scene == scene)
			{
				_startup_scene = scene;
				return true;
			}
		}

		return false;
	}

	RscHandle<Scene> SceneManager::GetSceneByBuildIndex(unsigned char index) const
	{
		if (index == 0x81)
			return _prefab_scene;

		for (auto& elem : _scenes)
		{
			if (elem.build_index == index)
				return elem.scene;
		}
		return RscHandle<Scene>{};
	}

	// fuck you C++
	span<SceneManager::SceneBlock const> SceneManager::GetScenes() const
	{
		return span<const SceneBlock>(_scenes);
	}

	RscHandle<Scene> SceneManager::CreateScene()
	{
		unsigned char build_index = 0;
		while (build_index < 0x80)
		{
			bool valid = true;
			for (auto& elem : _scenes)
			{
				if (elem.build_index == build_index)
				{
					valid = false;
					break;
				}
			}

			if (valid)
				break;
			else
				build_index++;
		}

		auto retval = Core::GetResourceManager().LoaderEmplaceResource<Scene>(build_index);
		_scenes.emplace_back(
			SceneBlock{ 
				build_index, 
				retval
			});

		return retval;
	}

	void SceneManager::Init()
	{
		Core::GetResourceManager().RegisterFactory<SceneFactory>();
		Core::GetResourceManager().RegisterLoader<SceneLoader>(Scene::ext);
	}

	void SceneManager::LateInit()
	{
		for (auto& elem : Core::GetSystem<FileSystem>().GetFilesWithExtension("/assets", Scene::ext, FS_FILTERS::ALL))
			Core::GetResourceManager().Load(elem);
		
		if (_active_scene)
			_active_scene->Load();

		_prefab_scene = Core::GetResourceManager().LoaderEmplaceResource<Scene>(uint8_t(0x81));
		_prefab_scene->Load();
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

	RscHandle<Scene> SceneManager::GetActiveScene()
	{
		return _active_scene;
	}

	RscHandle<Scene> SceneManager::GetPrefabScene()
	{
		return _prefab_scene;
	}

	bool SceneManager::SetActiveScene(RscHandle<Scene> s)
	{
		_active_scene = s;
		return true;
	}

	void SceneManager::DestroyObjects(span<GameObject> objs)
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

}