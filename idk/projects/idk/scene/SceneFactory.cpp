#include "stdafx.h"
#include "SceneFactory.h"
#include <res/MetaBundle.h>
#include <res/ResourceManager.h>
#include <core/GameState.inl>

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

	ResourceBundle SceneLoader::LoadFile(PathHandle, const MetaBundle& bundle)
	{

		const auto meta = bundle.FetchMeta<Scene>();
		return meta
			? Core::GetResourceManager().LoaderEmplaceResource<Scene>(meta->guid) 
			: Core::GetResourceManager().LoaderEmplaceResource<Scene>();
	}
}