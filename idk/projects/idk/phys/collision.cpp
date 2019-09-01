#include "stdafx.h"
#include "collision.h"
namespace idk
{

	col_result collide_ray_aabb(const ray& lhs, const aabb& bb)
	{
		col_result result{};
		auto disp_to_box = (bb.center() - lhs.origin);
		auto extents = bb.extents();
		result =  collide_ray_aabb_face<&vec3::x>(lhs.direction, disp_to_box, extents);
		auto tmp = collide_ray_aabb_face<&vec3::y>(lhs.direction, disp_to_box, extents);
		result = (tmp.dist >= 0 && tmp.dist < result.dist && tmp.perp_dist <= result.perp_dist) ? tmp : result;
		tmp = collide_ray_aabb_face<&vec3::z>(lhs.direction, disp_to_box, extents);
		result = (tmp.dist >= 0 &&tmp.dist < result.dist && tmp.perp_dist <= result.perp_dist) ? tmp : result;
		return result;
	}

}