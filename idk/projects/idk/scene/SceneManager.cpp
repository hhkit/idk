#include "stdafx.h"
#include "SceneManager.h"
#include <core/Core.h>
#include <scene/SceneFactory.h>

namespace idk
{
	void SceneManager::Init()
	{
		Core::GetSystem<ResourceManager>().RegisterFactory<SceneFactory>();
		_active_scene = Core::GetSystem<ResourceManager>().Create<Scene>();
		_active_scene->CreateGameObject();
	}

	void SceneManager::Shutdown()
	{
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

}