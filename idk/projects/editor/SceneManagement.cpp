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
#include <gfx/Camera.h>
#include <gfx/Light.h>
namespace idk
{
	void NewScene()
	{
		if (Core::GetSystem<SceneManager>().GetActiveScene())
			Core::GetSystem<SceneManager>().GetActiveScene()->Unload();
		Core::GetSystem<SceneManager>().SetActiveScene(Core::GetSystem<SceneManager>().CreateScene());
		auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene();
		active_scene->Load();

		// create default scene objects
		{
			auto light = active_scene->CreateGameObject();
			light->Name("Directional Light");
			light->Transform()->rotation = quat{ vec3{1,0,0}, deg{-90} };
			light->AddComponent<Light>()->light = DirectionalLight{ .light_color = {0.25, 0.25, 0.25} };
		}
		{
			auto camera = active_scene->CreateGameObject();
			camera->Name("Camera");
			camera->AddComponent<Camera>();
		}

		Core::GetSystem<IDE>().ClearScene();
	}
	bool OpenScene()
	{
		if (auto dialog_result = Core::GetSystem<Application>().OpenFileDialog({ "Scene", Scene::ext }))
		{
			auto virtual_path = Core::GetSystem<FileSystem>().ConvertFullToVirtual(*dialog_result);
			auto scene_res = Core::GetResourceManager().Get<Scene>(virtual_path);
			auto hnd = *scene_res;
			auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene();
			if (hnd != active_scene)
			{
				if (active_scene)
					active_scene->Unload();

				Core::GetSystem<SceneManager>().SetActiveScene(hnd);
				hnd->Load();

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

			Core::GetResourceManager().Rename(curr_scene, p);
			Core::GetResourceManager().Save(curr_scene);
			Core::GetSystem<ProjectManager>().SaveProject();
		}
	}
	void SaveSceneAs()
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

			Core::GetResourceManager().Rename(curr_scene, p);
			Core::GetResourceManager().Save(curr_scene);
			Core::GetSystem<ProjectManager>().SaveProject();
		}
	}
	void ResetScene()
	{
		if (const auto active_scene = Core::GetSystem<SceneManager>().GetActiveScene())
		{
			active_scene->Unload();
			active_scene->Load();
			Core::GetSystem<IDE>().ClearScene();
		}
	}
}