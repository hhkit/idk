#pragma once
#include <phys/collision_result.h>
#include <math/shapes.h>

namespace idk::phys
{
	namespace detail
	{
		template<decltype(&vec3::x) >
		float PerpDist(vec3 point_at_edge, vec3 extents);

		template<decltype(&vec3::x) axis>
		col_result collide_ray_aabb_face(vec3 direction, const vec3& disp_to_box, vec3 extents);
	}

	//Assumes lhs and rhs are normalized.
	col_result collide_ray_halfspace(const ray& lhs, const halfspace& rhs);

	col_result collide_ray_line(const ray& lhs, const ray& line);
	col_result collide_ray_aabb(const ray& lhs, const aabb& bb);
}

#include "collision_raycast.inl"