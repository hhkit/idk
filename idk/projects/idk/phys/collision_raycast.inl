#pragma once
#include "collision.h"
namespace idk::phys
{

	template<decltype(&vec3::x) axis>
	col_result collide_ray_aabb_face(vec3 direction, const vec3& disp_to_box, vec3 extents)
	{
		return col_failure{};
		//col_result result{};
		//const real t = direction.*axis / (disp_to_box.*axis - extents.*axis);
		//result.dist = t;
		//if (t >= 0)
		//{
		//	auto  point_at_edge = disp_to_box - direction * t;
		//	point_at_edge.*axis = 0;
		//	point_at_edge = abs(point_at_edge);
		//	auto pd_not_inside = PerpDist<axis>(point_at_edge, extents);
		//	//result.perp_dist = 
		//	if ((abs(point_at_edge.x) <= extents.x && abs(point_at_edge.y) <= extents.y && abs(point_at_edge.z) <= extents.z))
		//	{
		//		// success
		//		return col_success{};
		//	}
		//		? 0.0f : pd_not_inside;
		//}
		//return result;
	}
}