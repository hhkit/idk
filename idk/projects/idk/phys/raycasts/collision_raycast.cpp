#include "stdafx.h"
#include "collision_raycast.inl"
#include <ds/result.inl>
#include <gfx/DebugRenderer.h>
#include <iostream>

namespace idk::phys
{
	namespace detail
	{
		template<>
		float PerpDist<(&vec3::x)>(vec3 point_at_edge, vec3 extents)
		{
			const vec3 disp_from_corner = (point_at_edge - extents);

			return (disp_from_corner.y < 0) ? disp_from_corner.z : ((disp_from_corner.z < 0) ? disp_from_corner.y : disp_from_corner.length());
		}

		template<>
		float PerpDist<&vec3::y>(vec3 point_at_edge, vec3 extents)
		{
			const vec3 disp_from_corner = (point_at_edge - extents);

			return (disp_from_corner.x < 0) ? disp_from_corner.z : ((disp_from_corner.z < 0) ? disp_from_corner.x : disp_from_corner.length());
		}

		template<>
		float PerpDist<&vec3::z>(vec3 point_at_edge, vec3 extents)
		{
			const vec3 disp_from_corner = (point_at_edge - extents);

			return (disp_from_corner.y < 0) ? disp_from_corner.x : ((disp_from_corner.x < 0) ? disp_from_corner.y : disp_from_corner.length());
		}
	}

	raycast_result collide_ray_halfspace(const ray& l_ray, const halfspace& r_halfspace)
	{
		const auto disp = r_halfspace.origin_pt() - l_ray.origin;

		const auto normal_len = r_halfspace.normal.dot(disp);

		const auto normalized_normal = r_halfspace.normal;

		if (r_halfspace.contains(l_ray.origin)) // ray starts in half plane
		{
			raycast_success success;
			success.origin_colliding = true;
			success.distance_to_collision = 0;
			success.point_of_collision = l_ray.origin;
			return success;
		}

		const auto disp_dot_ray = disp.dot(l_ray.velocity);
		if (abs(normal_len) > epsilon) // ray is not parallel to plane
		{
			// otherwise ray is not in half plane
			
			// ray is moving away
			if (disp_dot_ray < -epsilon)
			{
				raycast_failure fail;
				fail.nearest_distance = normal_len;
				fail.perpendicular    = normalized_normal;
				fail.nearest_point    = l_ray.origin;
				return fail;
			}

			// ray is moving towards
			const auto ray_normalized = l_ray.velocity.get_normalized();

			raycast_success success;
			success.origin_colliding = false;
			success.distance_to_collision = normal_len;
			success.point_of_collision = ray_normalized.dot(normalized_normal) * normal_len * ray_normalized + l_ray.origin;

			return success;
		}
		else // ray is parallel to plane
		{
			// we already know that the halfplane does not contain the ray
			raycast_failure fail;
			fail.nearest_distance = disp_dot_ray;
			fail.perpendicular    = normalized_normal;
			fail.nearest_point    = l_ray.origin;
			return fail;
		}
	}

	//Assumes lhs and rhs are normalized.
	col_result collide_ray_line(const ray& lhs, const ray& line)
	{
		auto& rhs = line;
		//construct a plane around lhs
		const vec3 normal = lhs.velocity.cross(rhs.velocity);
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
			const vec3 nml_disp = (tangential_displacement - tangential_displacement.project_onto(rhs.velocity));
			const auto nml_len = nml_disp.length();
			const auto normalized_normal_disp = nml_disp / nml_len;

			const auto t = nml_len / lhs.velocity.dot(normalized_normal_disp);

			const vec3 tmp = lhs.velocity * t;
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
			const auto perp_vec = disp_to_rhs - disp_to_rhs.project_onto(lhs.velocity);
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

	raycast_result collide_ray_aabb(const ray& lhs, const aabb& box)
	{
		if (box.contains(lhs.origin))
		{
			raycast_success succ;
			succ.origin_colliding      = true;
			succ.distance_to_collision = 0;
			succ.point_of_collision    = lhs.origin;

			return succ;

		}

		auto disp_to_box = (lhs.origin - box.center());
		const auto extents = box.extents();

		auto result = detail::collide_ray_aabb_face<&vec3::x>(lhs, box);

		if (!result)
		{
			result = detail::collide_ray_aabb_face<&vec3::y>(lhs, box);

			if(!result)
				result = detail::collide_ray_aabb_face<&vec3::z>(lhs, box);
		}
		return result;
	}
	raycast_result collide_ray_box(const ray& lhs, const box& bb)
	{
		auto inverse_rot = bb.axes().transpose();
		
		// Get ray in box space where box is at 0,0,0
		ray box_space_ray;
		box_space_ray.origin = inverse_rot * (lhs.origin - bb.center);
		box_space_ray.velocity = inverse_rot * lhs.velocity;
		// Core::GetSystem<DebugRenderer>().Draw(ray{ box_space_ray.origin, box_space_ray.velocity * 100}, color{ 1,1,0 });

		// Min & max of box when its at 0,0,0 with no rotation.
		vec3 aabb_min{std::numeric_limits<float>::max()}, aabb_max{ std::numeric_limits<float>::min() };
		const auto half_extents = bb.half_extents();
		const auto n_half_extents = -bb.half_extents();
		aabb_min.x = min(n_half_extents.x, half_extents.x);
		aabb_min.y = min(n_half_extents.y, half_extents.y);
		aabb_min.z = min(n_half_extents.z, half_extents.z);

		aabb_max.x = max(n_half_extents.x, half_extents.x);
		aabb_max.y = max(n_half_extents.y, half_extents.y);
		aabb_max.z = max(n_half_extents.z, half_extents.z);

		aabb tmp{ aabb_min, aabb_max };
		auto result = collide_ray_aabb(box_space_ray, tmp);
		if (result)
		{
			// sphere s{};
			// Core::GetSystem<DebugRenderer>().Draw(sphere{ quat_cast<mat3>(bb.rotation) * result.value().point_of_collision });
			result.value().point_of_collision = quat_cast<mat3>(bb.rotation) * result.value().point_of_collision + bb.center;
		}
		return result;
	}
	raycast_result collide_ray_sphere(const ray& lhs, const sphere& s)
	{
		if (s.contains(lhs.origin))
		{
			return raycast_success{true,lhs.origin,0};
		}

		auto disp_to_sphere = (lhs.origin - s.center);

		//Calculating using quadratic equation formula (but this is just a fast way to test if ray collides, not resolution)
		const real	b = dot(disp_to_sphere, lhs.direction());
		const real	c = dot(disp_to_sphere, disp_to_sphere) - s.radius_sq();

		if (c > 0.f && b > 0.f)
		{
			//No collision occurred
			raycast_failure res;
			res.nearest_point = lhs.origin;
			res.nearest_distance = 0;
			return res;
		}

		const real discri = b * b - c;

		if (discri < 0.f)
		{
			//No collision occurred
			raycast_failure res;

			res.nearest_point = lhs.origin;
			res.nearest_distance = 0;
			return res;
		}

		const real	sq_discri = sqrtf(discri);
		real t = -b - sq_discri;

		//ray is in sphere
		if (t < 0.f)
			t = 0.f;

		//pt collision
		const vec3 poc = lhs.get_point_after(t);

		return raycast_success{false,poc,lhs.origin.distance(poc) };
	}


	raycast_result collide_ray_capsule(const ray& lhs, const capsule& c)
	{
		const auto& capsule_line_points = c.get_line_points();
		const auto& closest_pt_pair = ClosestPairPointsLineSegmentToLineSegment(lhs.origin,lhs.origin+lhs.velocity, capsule_line_points.first,capsule_line_points.second);
		const vec3 vector_line_capsule = closest_pt_pair.first - closest_pt_pair.second; //Line to capsule vector
		auto distance = vector_line_capsule.length();
		if (epsilon_equal(closest_pt_pair.first.length_sq(), closest_pt_pair.second.length_sq()) || c.radius >= distance) { //Lines intersect OR //Lines are close such that the capsule radius intersect
			float s_to_sphere = lhs.direction().get_normalized().dot(closest_pt_pair.second - lhs.origin) - c.radius;
			const auto point_of_collision = lhs.origin + lhs.direction().get_normalized()* s_to_sphere;
		
			return raycast_success{ false,point_of_collision,lhs.origin.distance(point_of_collision) };
		}

		return raycast_failure { lhs.origin };

	}
}