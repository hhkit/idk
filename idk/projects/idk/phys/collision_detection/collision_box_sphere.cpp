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
			if (normal_len > epsilon)
			{
				normal /= normal_len;
				return normal_len;
			}

			auto dist = half_extents.x - circle_local_pos.x;
			auto min_dist = dist;
			closest_pt.x = half_extents.x;
			normal = vec3{ 1,0,0 };

			dist = half_extents.x + circle_local_pos.x;
			if (dist < min_dist)
			{
				min_dist = dist;
				closest_pt = circle_local_pos;
				closest_pt.x = -half_extents.x;
				normal = vec3{ -1,0,0 };
			}

			dist = half_extents.y - circle_local_pos.y;
			if (dist < min_dist)
			{
				min_dist = dist;
				closest_pt = circle_local_pos;
				closest_pt.y = half_extents.y;
				normal = vec3{ 0,1,0 };
			}

			dist = half_extents.y + circle_local_pos.y;
			if (dist < min_dist)
			{
				min_dist = dist;
				closest_pt = circle_local_pos;
				closest_pt.y = -half_extents.y;
				normal = vec3{ 0,-1,0 };
			}

			dist = half_extents.z - circle_local_pos.z;
			if (dist < min_dist)
			{
				min_dist = dist;
				closest_pt = circle_local_pos;
				closest_pt.z = half_extents.z;
				normal = vec3{ 0,0,1 };
			}

			dist = half_extents.z + circle_local_pos.z;
			if (dist < min_dist)
			{
				min_dist = dist;
				closest_pt = circle_local_pos;
				closest_pt.z = -half_extents.z;
				normal = vec3{ 0,0,-1 };
			}

			return min_dist;
		}();

		col_success result;

		const auto penetration_depth = distance - intersection_dist;

		normal = lhs.axes * normal; // put into world space
		result.normal_of_collision = -normal;
		result.penetration_depth = penetration_depth;

		return result;
	}
}
