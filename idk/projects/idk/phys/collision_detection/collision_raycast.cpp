#include <stdafx.h>
#include "collision_raycast.h"

namespace idk::phys
{
	namespace detail
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

		template<>
		float PerpDist<&vec3::z>(vec3 point_at_edge, vec3 extents)
		{
			vec3 disp_from_corner = (point_at_edge - extents);

			return (disp_from_corner.y < 0) ? disp_from_corner.x : ((disp_from_corner.x < 0) ? disp_from_corner.y : disp_from_corner.length());
		}
	}

	col_result collide_ray_halfspace(const ray& l_ray, const halfspace& r_halfspace)
	{
		const auto disp = r_halfspace.origin_pt() - l_ray.origin;

		const auto normal_len = r_halfspace.normal.dot(disp);

		const auto normalized_normal = r_halfspace.normal;

		if (r_halfspace.contains(l_ray.origin)) // ray starts in half plane
		{
			col_success success;
			success.normal_of_collision = normalized_normal;
			success.penetration_depth = normal_len;
			success.point_of_collision = l_ray.origin;
			return success;
		}

		const auto disp_dot_ray = disp.dot(l_ray.direction);
		if (abs(normal_len) > epsilon) // ray is not parallel to plane
		{
			// otherwise ray is not in half plane
			
			// ray is moving away
			if (disp_dot_ray < -epsilon)
			{
				col_failure fail;
				fail.perp_dist = normal_len;
				fail.separating_axis = normalized_normal;
				return fail;
			}

			// ray is moving towards
			const auto ray_normalized = l_ray.direction.get_normalized();

			col_success success;
			success.normal_of_collision = normalized_normal;
			success.penetration_depth = normal_len;
			success.point_of_collision = ray_normalized.dot(normalized_normal) * normal_len * ray_normalized + l_ray.origin;

			return success;
		}
		else // ray is parallel to plane
		{
			// we already know that the halfplane does not contain the ray
			col_failure fail;
			fail.perp_dist = disp_dot_ray;
			fail.separating_axis = normalized_normal;
			return fail;
		}
	}

	//Assumes lhs and rhs are normalized.
	col_result collide_ray_line(const ray& lhs, const ray& line)
	{
		auto& rhs = line;
		//construct a plane around lhs
		const vec3 normal = lhs.direction.cross(rhs.direction);
		const vec3 disp_to_rhs = rhs.origin - lhs.origin;

		const auto normal_len_sq = normal.length_sq();

		if (normal_len_sq > epsilon) // ray and line point differently
		{
			const auto normalized_n = normal / sqrt(normal_len_sq);	 // direction of difference
			const auto perp_dist = disp_to_rhs.dot(normalized_n);    // perp distance between ray and line

			if (abs(perp_dist) > epsilon) // rays are not on the same plane
			{
				const auto tangential_disp = disp_to_rhs - disp_to_rhs.dot(normalized_n) * normalized_n; // remove normal component

				// put it back on the line
			}

			const auto tangential_displacement =
				epsilon_equal(perp_dist, 0)
				? disp_to_rhs
				: disp_to_rhs - disp_to_rhs.dot(normalized_n) * normalized_n; // rays not on same plane, remove perp component

			//rays are on the same plane
			vec3 nml_disp = (tangential_displacement - tangential_displacement.project_onto(rhs.direction));
			const auto nml_len = nml_disp.length();
			const auto normalized_normal_disp = nml_disp / nml_len;

			const auto t = nml_len / lhs.direction.dot(normalized_normal_disp);

			vec3 tmp = lhs.direction * t;
			const auto dist = t;

			if (dist > -epsilon)
			{
				col_success succ;
				succ.point_of_collision = lhs.origin + tmp;
				return succ;
			}
			else
			{
				col_failure fail;
				fail.perp_dist = t;
				fail.separating_axis = nml_disp;
				return fail;
			}
		}
		else // ray is parallel to line
		{
			const auto perp_vec = disp_to_rhs - disp_to_rhs.project_onto(lhs.direction);
			const auto perp_dist = perp_vec.length();
			if (epsilon_equal(perp_dist, 0))
			{
				col_success result;
				result.point_of_collision = lhs.origin;
				return result;
			}
			else
			{
				constexpr auto parallel_dist = std::numeric_limits<float>::infinity();
				col_failure result;
				result.separating_axis = perp_vec / perp_dist;
				result.perp_dist = perp_dist;
				return result;
			}
		}
	}

	col_result collide_ray_aabb(const ray& lhs, const aabb& box)
	{
		auto disp_to_box = (box.center() - lhs.origin);
		auto extents = box.extents();

		auto result = detail::collide_ray_aabb_face<&vec3::x>(lhs.direction, disp_to_box, extents);

		if (auto tmp_res = detail::collide_ray_aabb_face<&vec3::y>(lhs.direction, disp_to_box, extents))
		{
			if (result && result.success().penetration_depth > tmp_res.success().penetration_depth)
				result = tmp_res;
			// if result already failed ignore the successful axis
		}
		else
		{
			if (result)
				result = tmp_res;
			else
			{
				if (tmp_res.failure().perp_dist < result.failure().perp_dist)
					result = tmp_res;
			}
		}

		if (auto tmp_res = detail::collide_ray_aabb_face<&vec3::z>(lhs.direction, disp_to_box, extents))
		{
			if (result && result.success().penetration_depth > tmp_res.success().penetration_depth)
				result = tmp_res;
			// if result already failed ignore the successful axis
		}
		else
		{
			if (result)
				result = tmp_res;
			else
			{
				if (tmp_res.failure().perp_dist < result.failure().perp_dist)
					result = tmp_res;
			}
		}

		return result;
	}
}