#include "stdafx.h"
#include "GameState.h"
#include <core/ComponentIncludes.h>

namespace idk
{
	GameState::GameState()
		: _objects{detail::ObjectPools::Instantiate()}
	{
		_instance = this;
	}

	GameState::~GameState() = default;

	opt<Scene> GameState::ActivateScene(uint8_t scene)
	{
		if (detail::ObjectPools::ActivateScene(_objects, scene))
			return Scene{ scene };
		else
			return {};
	}
	bool GameState::DectivateScene(uint8_t scene)
	{
		return detail::ObjectPools::DeactivateScene(_objects, scene);
	}
	bool GameState::DectivateScene(Scene scene)
	{
		return DectivateScene(scene.scene_id);
	}
	GameState& GameState::GetGameState()
	{
		return *_instance;
	}
}
