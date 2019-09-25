#include "stdafx.h"
#include "SceneManager.h"
#include <core/Core.h>
#include <scene/SceneFactory.h>
#include <serialize/serialize.h>
#include <util/ioutils.h>

namespace idk
{
	SceneManager::SceneActivateResult SceneManager::ActivateScene(RscHandle<class Scene> activateme)
	{
		if (activateme)
			return SceneActivateResult::Err_SceneAlreadyActive;

		for (auto& elem : _scenes)
		{
			if (elem.scene == activateme)
			{
				/*
				auto paths = Core::GetSystem<FileSystem>().GetFilesWithExtension("/assets", Scene::ext);
				for (auto& path : paths)
				{
					auto meta_file = PathHandle(string{ path.GetMountPath() } +".meta");
					auto meta_stream = meta_file.Open(FS_PERMISSIONS::READ);
					auto meta = parse_text<MetaFile>(stringify(meta_stream));

					if (meta.guids[0] == activateme.guid)
					{
						auto scene = Core::GetResourceManager().Emplace<Scene>(activateme.guid, elem.build_index);
						auto file_stream = path.Open(FS_PERMISSIONS::READ);
						parse_text(stringify(file_stream), *scene);
						return SceneActivateResult::Ok;
					}
				}
				*/
				return SceneActivateResult::Err_ScenePathNotFound;
			}
		}
		return SceneActivateResult::Err_SceneNotInProject;
	}

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
	void SceneManager::Shutdown()
	{
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

		auto guid = Guid::Make();
		_scenes.emplace_back(SceneBlock{ build_index, RscHandle<Scene>{guid} });

		return Core::GetResourceManager().LoaderEmplaceResource<Scene>(guid, build_index);
	}

	void SceneManager::Init()
	{
		Core::GetSystem<ResourceManager>().RegisterFactory<SceneFactory>();
	}

	RscHandle<Scene> SceneManager::GetActiveScene()
	{
		return _active_scene;
	}

	bool SceneManager::SetActiveScene(RscHandle<Scene> s)
	{
		_active_scene = s;
		return true;
	}

	void SceneManager::DestroyObjects()
	{
		GameState::GetGameState().DestroyQueue();
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