#include "stdafx.h"
#include "GameState.h"
#include <core/ComponentIncludes.h>

namespace idk
{
	GameState::GameState()
		: _objects{detail::ObjectPools::Instantiate()}
	{
	}

	GameState::~GameState() = default;
}
