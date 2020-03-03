#pragma once
#include <idk.h>

namespace idk::phys
{
	struct collision_transform
	{
		vec3 position;
		mat3 rotation;
	};
}