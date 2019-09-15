#pragma once
#include "collision_result.h"
#include <math/shapes/sphere.h>

namespace idk::phys
{
	col_result collide_sphere_sphere_discrete(const sphere& lhs, const sphere& rhs);
}