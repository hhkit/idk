#pragma once
#include "collision.h"
namespace idk
{

	template<>
	float PerpDist<(&vec3::x)>(vec3 point_at_edge, vec3 extents)
	{
		vec3 disp_from_corner = (point_at_edge - extents);

		return (disp_from_corner.y < 0) ? disp_from_corner.z : ((disp_from_corner.z < 0) ? disp_from_corner.y : disp_from_corner.length());
	}

	template<>
	float PerpDist<&vec3::y>(vec3 point_at_edge, vec3 extents)
	{
		vec3 disp_from_corner = (point_at_edge - extents);

		return (disp_from_corner.x < 0) ? disp_from_corner.z : ((disp_from_corner.z < 0) ? disp_from_corner.x : disp_from_corner.length());
	}

	template< >
	float PerpDist<&vec3::z>(vec3 point_at_edge, vec3 extents)
	{
		vec3 disp_from_corner = (point_at_edge - extents);

		return (disp_from_corner.y < 0) ? disp_from_corner.x : ((disp_from_corner.x < 0) ? disp_from_corner.y : disp_from_corner.length());
	}
	template<decltype(&vec3::x) axis>
	col_result collide_ray_aabb_face(vec3 direction, const vec3& disp_to_box, vec3 extents)
	{

		col_result result{};
		float t = direction.*axis / (disp_to_box.*axis - extents.*axis);
		result.dist = t;
		if (t >= 0)
		{
			auto  point_at_edge = disp_to_box - direction * t;
			point_at_edge.*axis = 0;
			point_at_edge = vabs(point_at_edge);
			auto pd_not_inside = PerpDist<axis>(point_at_edge, extents);
			result.perp_dist = ((abs(point_at_edge.x) <= extents.x && abs(point_at_edge.y) <= extents.y && abs(point_at_edge.z) <= extents.z))
				? 0.0f : pd_not_inside;
		}
		return result;
	}
}