#include "pch.h"
#include "SceneManagement.h"
#include <app/Application.h>
#include <proj/ProjectManager.h>
#include <core/GameObject.inl>
#include <common/TagManager.h>
#include <common/Tag.h>
#include <common/Transform.h>
#include <IDE.h>
#include <scene/SceneManager.h>
#include <gfx/Camera.h>
#include <gfx/Light.h>
#include <script/ScriptSystem.h>
#include <res/ResourceHandle.inl>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceUtils.inl>
#include <serialize/text.inl>
#include <res/Guid.inl>

#include <util/ioutils.h>
#include <ds/result.inl>

namespace idk
{
	void NewScene()
	{
        Core::GetSystem<IDE>().ClearScene();

		if (Core::GetSystem<SceneManager>().GetActiveScene())
			Core::GetSystem<SceneManager>().GetActiveScene()->Deactivate();
		Core::GetSystem<SceneManager>().SetActiveScene(Core::GetResourceManager().Create<Scene>());
		auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene();
		active_scene->LoadFromResourcePath();
		Core::GetSystem<IDE>().curr_scene = active_scene;

		// create default scene objects
		{
			auto light = active_scene->CreateGameObject();
			light->Name("Directional Light");
			light->Transform()->rotation = quat{ vec3{1,0,0}, deg{-90} };
			light->AddComponent<Light>()->light = DirectionalLight{};
		}
		{
			auto camera = active_scene->CreateGameObject();
			camera->Name("Camera");
            camera->AddComponent<Camera>();
		}

        Core::GetSystem<IDE>().reg_scene.set("scene", string{ active_scene.guid });
	}

	bool OpenScene()
	{
		if (const auto dialog_result = Core::GetSystem<Application>().OpenFileDialog({ "Scene", Scene::ext }))
		{
            const auto virtual_path = Core::GetSystem<FileSystem>().ConvertFullToVirtual(*dialog_result);
            const auto scene_res = Core::GetResourceManager().Get<Scene>(virtual_path);
            return OpenScene(*scene_res);
		}
		return false;
	}

    bool OpenScene(RscHandle<Scene> scene)
    {
        if (!scene)
            return false;

        Core::GetSystem<IDE>().curr_scene = scene;
        Core::GetSystem<SceneManager>().SetNextScene(scene);

        Core::GetSystem<IDE>().ClearScene();
        Core::GetSystem<IDE>().reg_scene.set("scene", string{ scene.guid });

        return true;
    }

	void SaveScene()
	{
		if (Core::GetSystem<IDE>().IsGameRunning())
			return;

		auto curr_scene = Core::GetSystem<IDE>().curr_scene;
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

			Core::GetResourceManager().Rename(curr_scene, p);
			Core::GetResourceManager().Save(curr_scene);
			curr_scene->Activate();
			Core::GetSystem<ProjectManager>().SaveProject();
		}
	}

	void SaveSceneAs()
	{
		auto curr_scene = Core::GetSystem<IDE>().curr_scene;
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

			auto res = Core::GetResourceManager().CopyTo(curr_scene, p);
			if (res)
				Core::GetSystem<SceneManager>().SetNextScene(res.value());
			Core::GetSystem<ProjectManager>().SaveProject();
		}
	}

	void SaveSceneTemporarily()
	{
		if (const auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene())
		{
			auto stream = Core::GetSystem<FileSystem>().Open(IDE::path_tmp_scene, FS_PERMISSIONS::WRITE);
			stream << serialize_text(*active_scene);
		}
	}

	void RestoreFromTemporaryScene()
	{
		if (const auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene())
		{
			active_scene->Deactivate();
			auto load_scene = Core::GetSystem<IDE>().curr_scene;
			Core::GetSystem<SceneManager>().SetActiveScene(load_scene);
			Core::GetGameState().DeactivateScene(0);
			load_scene->Activate();
			auto stream = Core::GetSystem<FileSystem>().Open(IDE::path_tmp_scene, FS_PERMISSIONS::READ);
			auto deser = stringify(stream);
			parse_text(deser, *load_scene);
			Core::GetSystem<IDE>().ClearScene();
		}
	}

	void HotReloadDLL()
	{
		if (const auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene())
		{
			const auto prefab_scene =Core::GetSystem<SceneManager>().GetPrefabScene();

			auto objs = Core::GetSystem<IDE>().GetSelectedObjects();
			SaveSceneTemporarily();
			active_scene->Deactivate();
			prefab_scene->Deactivate();
			for (auto& path : Core::GetSystem<FileSystem>().GetEntries("/assets", FS_FILTERS::FILE | FS_FILTERS::RECURSE_DIRS, ".idp"))
				if (path.GetExtension() == ".idp")
					Core::GetResourceManager().Unload(path);


			Core::GetSystem<mono::ScriptSystem>().RefreshGameScripts();
			prefab_scene->LoadFromResourcePath();
			for (auto& path : Core::GetSystem<FileSystem>().GetEntries("/assets", FS_FILTERS::FILE | FS_FILTERS::RECURSE_DIRS, ".idp"))
				if (path.GetExtension() == ".idp")
					Core::GetResourceManager().Load(path, true);


			RestoreFromTemporaryScene();
			Core::GetSystem<IDE>().SetSelection(objs);

		}
	}
}