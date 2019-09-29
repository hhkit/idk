#pragma once
#include <phys/collision_result.h>
#include <math/shapes/box.h>
#include <math/shapes/sphere.h>

namespace idk::phys
{
	col_result collide_box_sphere_discrete(const box& lhs, const sphere& rhs);
	col_result collide_box_sphere_continuous(const box& lhs, const vec3& lvel, const sphere& rhs, const vec3& rvel);
}