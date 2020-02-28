#include "stdafx.h"
#include "collision_box_sphere.h"
#include <phys/collision_rotational.h>
namespace idk::phys
{
	col_result collide_box_sphere_discrete(const box& lhs, const sphere& rhs)
	{
		// convert sphere into box space 
		const auto l_axes = lhs.axes();

		const auto circle_local_pos = l_axes.transpose() * (rhs.center - lhs.center);
		const auto half_extents = lhs.half_extents();

		auto closest_pt = circle_local_pos;

		closest_pt.x = std::clamp(closest_pt.x, -half_extents.x, half_extents.x);
		closest_pt.y = std::clamp(closest_pt.y, -half_extents.y, half_extents.y);
		closest_pt.z = std::clamp(closest_pt.z, -half_extents.z, half_extents.z);

		const float distance_sq = circle_local_pos.distance_sq(closest_pt);
		if (distance_sq > rhs.radius * rhs.radius)
		{
			col_failure error;
			error.perp_dist = sqrt(distance_sq);
			error.separating_axis = (l_axes * closest_pt).get_normalized();
			return error;
		}

		col_success result;

		const auto distance = sqrtf(distance_sq);

		result.centerA = lhs.center;
		result.centerB = rhs.center;
		result.contactCount = 1;
		if (distance < epsilon)
		{
			result.contacts[0].penetration = -rhs.radius;
			result.contacts[0].position = rhs.center;
			result.normal = vec3{0, 1, 0};
		}
		else
		{
			const auto penetration_depth = distance - rhs.radius;
			result.contacts[0].penetration = penetration_depth;
			result.contacts[0].position = (mat3{ l_axes } * closest_pt) + lhs.center;
			result.normal = (rhs.center - result.contacts[0].position).get_normalized();
			// result.contacts[0].position = rhs.center - result.normal * rhs.radius;
		}
		result.max_penetration = result.contacts[0].penetration;
		compute_basis(result.normal, result.tangentVectors, result.tangentVectors + 1);
		// LOG("Norm: %f, %f, %f", result.normal.x, result.normal.y, result.normal.z);
		return result;
	}
}
