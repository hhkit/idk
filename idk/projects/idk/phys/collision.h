#pragma once
#include <idk.h>
#include <shape/ray.h>
#include <shape/aabb.h>
#include <shape/capsule.h>
#include <shape/sphere.h>
namespace idk
{
	bool epsilon_equal(float lhs, float rhs);
	struct col_result
	{
		float perp_dist=-1.0f;//default false.
		float dist     = 0.0f;
		operator bool()const { return epsilon_equal(perp_dist, 0)&& !(dist==std::numeric_limits<float>::infinity()) && dist >=-epsilon ; }
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