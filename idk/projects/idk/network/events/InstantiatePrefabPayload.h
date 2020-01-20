#pragma once
#include <idk.h>
#include <res/ResourceHandle.h>

namespace idk
{
	struct EventInstantiatePrefabPayload
	{
		unsigned view_id = 0;

		RscHandle<Prefab> prefab;
		bool has_position;
		vec3 position;
		bool has_rotation;
		quat rotation;
	};
}