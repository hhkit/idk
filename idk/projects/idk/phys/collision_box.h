#pragma once
#include "collision_result.h"
#include <math/shapes/box.h>

namespace idk::phys
{
	col_result collide_box_box_discrete(const box& lhs, const box& rhs);
	col_result collide_box_box_continuous(const box& lhs, const vec3& lvel, const box& rhs, const vec3& rvel);
}