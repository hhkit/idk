#include "stdafx.h"
#include "SceneFactory.h"
#include <res/MetaBundle.h>
#include <scene/SceneManager.h>

namespace idk
{
	unique_ptr<Scene> SceneFactory::GenerateDefaultResource()
	{
		auto retval = std::make_unique<Scene>(Scene::editor);
		GameState::GetGameState().ActivateScene(Scene::editor);
		return retval;
	}

	unique_ptr<Scene> SceneFactory::Create() noexcept
	{
		return std::make_unique<Scene>();
	}

	unique_ptr<Scene> SceneFactory::Create(PathHandle h) noexcept
	{
		Core::GetGameState().ActivateScene(0);
		return std::make_unique<Scene>();
	}
}