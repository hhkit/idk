#include "stdafx.h"
#include "Scene.h"

namespace idk
{
	Scene::Scene(uint8_t scene_id)
		: scene_id{ scene_id }
	{
	}

	Scene GetScene(const GenericHandle& handle)
	{
		return Scene{ handle.scene };
	}
}