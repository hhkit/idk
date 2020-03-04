#pragma once
#include <idk.h>

namespace idk::phys
{
	struct ColliderInfo;

	struct ContactPoint
	{
		vec3 position{ 0, 0, 0 };				// World coordinate of contact
		float penetration{0};					// Depth of penetration from collision
	};
}