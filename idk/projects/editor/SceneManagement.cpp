#include "pch.h"
#include "SceneManagement.h"
#include <app/Application.h>
#include <proj/ProjectManager.h>
#include <core/GameObject.h>
#include <common/TagManager.h>
#include <common/Tag.h>
#include <common/Transform.h>
#include <IDE.h>
#include <scene/SceneManager.h>
#include <res/compiler/AssetImporter.h>
#include <editor/compiler/IDEAssetImporter.h>
#include <gfx/Camera.h>
#include <gfx/Light.h>
#include <script/ScriptSystem.h>

#include <util/ioutils.h>

namespace idk
{
	void NewScene()
	{
		if (Core::GetSystem<SceneManager>().GetActiveScene())
			Core::GetSystem<SceneManager>().GetActiveScene()->Deactivate();
		Core::GetSystem<SceneManager>().SetActiveScene(Core::GetResourceManager().Create<Scene>());
		auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene();
		active_scene->LoadFromResourcePath();

		// create default scene objects
		{
			auto light = active_scene->CreateGameObject();
			light->Name("Directional Light");
			light->Transform()->rotation = quat{ vec3{1,0,0}, deg{-90} };
			light->AddComponent<Light>()->light = DirectionalLight{};
		}
		//{
		//	auto camera = active_scene->CreateGameObject();
		//	camera->Name("Camera");
		//	camera->AddComponent<Camera>();
		//}

		Core::GetSystem<IDE>().ClearScene();
	}
	bool OpenScene()
	{
		if (auto dialog_result = Core::GetSystem<Application>().OpenFileDialog({ "Scene", Scene::ext }))
		{
			auto virtual_path = Core::GetSystem<FileSystem>().ConvertFullToVirtual(*dialog_result);
			auto hnd = Core::GetSystem<AssetImporter>().Get<Scene>(virtual_path);
			auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene();
			if (hnd != active_scene)
			{
				if (active_scene)
					active_scene->Deactivate();

				Core::GetSystem<SceneManager>().SetActiveScene(hnd);
				hnd->LoadFromResourcePath();

				Core::GetSystem<IDE>().ClearScene();
				return true;
			}
		}
		return false;
	}
	void SaveScene()
	{
		auto curr_scene = Core::GetSystem<SceneManager>().GetActiveScene();
		auto path = [&]() -> opt<string>
		{
			if (auto path = Core::GetResourceManager().GetPath(curr_scene))
				return string{ *path };
			else
			{
				auto dialog_result = Core::GetSystem<Application>().OpenFileDialog({ "Scene", Scene::ext, DialogType::Save });
				if (dialog_result)
					return Core::GetSystem<FileSystem>().ConvertFullToVirtual(*dialog_result);
				else
					return std::nullopt;
			}
		}();

		if (path)
		{
			auto& p = *path;
			if (p.find(Scene::ext) == std::string::npos)
				p += Scene::ext;

			Core::GetSystem<EditorAssetImporter>().Rename(curr_scene, p);
			Core::GetSystem<EditorAssetImporter>().Save(curr_scene);
			Core::GetSystem<ProjectManager>().SaveProject();
		}
	}
	void SaveSceneAs()
	{
		auto curr_scene = Core::GetSystem<SceneManager>().GetActiveScene();
		auto path = [&]() -> opt<string>
		{
			auto dialog_result = Core::GetSystem<Application>().OpenFileDialog({ "Scene", Scene::ext, DialogType::Save });
			if (dialog_result)
				return Core::GetSystem<FileSystem>().ConvertFullToVirtual(*dialog_result);
			else
				return std::nullopt;
		}();

		if (path)
		{
			auto& p = *path;
			if (p.find(Scene::ext) == std::string::npos)
				p += Scene::ext;

			auto res = Core::GetSystem<EditorAssetImporter>().CopyTo(curr_scene, p);
			if (res)
			{
				curr_scene->Deactivate();
				Core::GetSystem<SceneManager>().SetActiveScene(res.value());
				Core::GetSystem<SceneManager>().GetActiveScene()->LoadFromResourcePath();
			}
			Core::GetSystem<ProjectManager>().SaveProject();
		}
	}

	void SaveSceneTemporarily()
	{
		if (const auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene())
		{
			auto stream = Core::GetSystem<FileSystem>().Open(Core::GetSystem<IDE>().GetTmpSceneMountPath(), FS_PERMISSIONS::WRITE);
			stream << serialize_text(*active_scene);
		}
	}

	void RestoreFromTemporaryScene()
	{
		if (const auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene())
		{
			active_scene->Deactivate();
			active_scene->Activate();
			auto stream = Core::GetSystem<FileSystem>().Open(Core::GetSystem<IDE>().GetTmpSceneMountPath(), FS_PERMISSIONS::READ);
			auto deser = stringify(stream);
			parse_text(deser, *active_scene);
			Core::GetSystem<IDE>().ClearScene();
		}
	}

	void HotReloadDLL()
	{
		if (const auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene())
		{
			const auto prefab_scene =Core::GetSystem<SceneManager>().GetPrefabScene();
			SaveSceneTemporarily();
			active_scene->Deactivate();
			prefab_scene->Deactivate();
			for (auto& path : Core::GetSystem<FileSystem>().GetEntries("/assets", FS_FILTERS::FILE | FS_FILTERS::RECURSE_DIRS, ".idp"))
				if (path.GetExtension() == ".idp")
					for (auto& prefab : Core::GetSystem<AssetImporter>().Get(path).GetAll<Prefab>())
						Core::GetResourceManager().Destroy(prefab);

			Core::GetSystem<mono::ScriptSystem>().RefreshGameScripts();

			RestoreFromTemporaryScene();
			prefab_scene->LoadFromResourcePath();

			for (auto& path : Core::GetSystem<FileSystem>().GetEntries("/assets", FS_FILTERS::FILE | FS_FILTERS::RECURSE_DIRS, ".idp"))
				if (path.GetExtension() == ".idp")
					Core::GetResourceManager().Load<Prefab>(path, true);
		}
	}
}