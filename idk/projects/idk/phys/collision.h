#pragma once
#include <idk.h>
#include <math/shapes/ray.h>
#include <math/shapes/aabb.h>
#include <math/shapes/capsule.h>
#include <math/shapes/sphere.h>
namespace idk
{
	bool epsilon_equal(real lhs, real rhs);

	struct col_result
	{
		real perp_dist = -1.0f; // default false.
		real dist      =  0.0f;
		vec3 separating_axis{};
		explicit operator bool() const;
	};

	//Assumes lhs and rhs are normalized.
	col_result collide_ray_line(const ray& lhs, const ray& line);
	template<decltype(&vec3::x) >
	float PerpDist(vec3 point_at_edge, vec3 extents);

	template<decltype(&vec3::x) axis>
	col_result collide_ray_aabb_face(vec3 direction, const vec3& disp_to_box, vec3 extents);
	col_result collide_ray_aabb(const ray& lhs, const aabb& bb);
}
#include "collision.inl"