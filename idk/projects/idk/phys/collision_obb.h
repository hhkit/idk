#pragma once
#include "collision_result.h"
#include <math/shapes/box.h>

namespace idk::phys
{
	col_result collide_box_box_static(const box& lhs, const box& rhs);
	col_result collide_box_box_dynamic(const box& lhs, const vec3& lvel, const box& rhs, const vec3& rvel);
}