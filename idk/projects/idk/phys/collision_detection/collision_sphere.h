#pragma once
#include <phys/collision_result.h>
#include <math/shapes/sphere.h>

namespace idk::phys
{
	col_result collide_sphere_sphere_discrete(const sphere& lhs, const sphere& rhs);
	col_result collide_sphere_sphere_continuous(const sphere& lhs, const vec3& lvel, const sphere& rhs, const vec3& rvel);
}