#include "stdafx.h"
#include "ProjectManager.h"
#include <serialize/serialize.h>
#include <util/ioutils.h>

namespace idk
{
	ProjectManager::SceneActivateResult ProjectManager::ActivateScene(RscHandle<class Scene> activateme)
	{
		if (activateme)
			return SceneActivateResult::Err_SceneAlreadyActive;

		for (auto& elem : _scenes)
		{
			if (elem.scene == activateme)
			{ 
				auto paths = Core::GetSystem<FileSystem>().GetFilesWithExtension("/assets", Scene::ext);
				for (auto& path : paths)
				{
					auto meta_file = FileHandle(string{ path.GetMountPath() } +".meta");
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

				return SceneActivateResult::Err_ScenePathNotFound;
			}
		}
		return SceneActivateResult::Err_SceneNotInProject;
	}

	RscHandle<Scene> ProjectManager::StartupScene() const
	{
		return _startup_scene;
	}

	bool ProjectManager::StartupScene(RscHandle<Scene> scene)
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

	RscHandle<Scene> ProjectManager::GetSceneByBuildIndex(unsigned char index) const
	{
		for (auto& elem : _scenes)
		{
			if (elem.build_index == index)
				return elem.scene;
		}
		return RscHandle<Scene>{};
	}

	// fuck you C++
	span<ProjectManager::SceneBlock const> ProjectManager::GetScenes() const
	{
		return span<const SceneBlock>(_scenes);
	}

	void ProjectManager::Init()
	{
	}

	void ProjectManager::LateInit()
	{
		Core::GetSystem<FileSystem>().Open(project_path, FS_PERMISSIONS::READ);
		// initialize project
	}

	void ProjectManager::Shutdown()
	{
	}

	RscHandle<Scene> ProjectManager::CreateScene()
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

		return Core::GetResourceManager().Emplace<Scene>(guid, build_index);
	}
}
