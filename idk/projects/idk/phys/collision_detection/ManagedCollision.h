#pragma once
#include <idk.h>

namespace idk
{
	struct ManagedCollision
	{
		uint64_t collider_id;
		vec3 normal;
		vec3 contact_pt;
	};
}