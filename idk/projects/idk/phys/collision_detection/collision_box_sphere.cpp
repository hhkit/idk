#include "stdafx.h"
#include "collision_box_sphere.h"

namespace idk::phys
{
	col_result collide_box_sphere_discrete(const box& lhs, const sphere& rhs)
	{
		// convert sphere into box space 
		const auto circle_local_pos = lhs.axes.inverse() * (rhs.center - lhs.center);
		const auto half_extents = lhs.half_extents();

		auto closest_pt = circle_local_pos;

		closest_pt.x = std::clamp(closest_pt.x, -half_extents.x, half_extents.x);
		closest_pt.y = std::clamp(closest_pt.y, -half_extents.y, half_extents.y);
		closest_pt.z = std::clamp(closest_pt.z, -half_extents.z, half_extents.z);

		const auto intersection_dist = rhs.radius;

		auto normal = circle_local_pos - closest_pt;
		const auto normal_len_sq = normal.length_sq();
		const auto normal_len = sqrt(normal_len_sq);

		if (normal_len_sq > intersection_dist * intersection_dist)
		{
			col_failure error;
			error.perp_dist = normal_len;
			error.separating_axis = lhs.axes * normal / normal_len;
			return error;
		}

		
		// todo: handle case where sphere is inside box

		const auto distance = [&]() -> real
		{
			return normal_len;
		}();

		col_success result;

		const auto penetration_depth = distance - intersection_dist;

		normal /= normal_len;
		normal = lhs.axes * normal; // put into world space
		result.normal_of_collision = -normal;

		return result;
	}
}
