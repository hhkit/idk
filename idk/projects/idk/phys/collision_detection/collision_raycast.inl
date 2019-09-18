#pragma once
#include <phys/collision_result.h>
namespace idk::phys
{
	namespace detail
	{
		template<decltype(&vec3::x) axis>
		raycast_result collide_ray_aabb_face(const ray& l_ray, const aabb& r_aabb)
		{
			// time to collide
			constexpr auto nml = []()
			{
				if constexpr(axis == &vec3::x) return vec3{ 1,0,0 };
				if constexpr(axis == &vec3::y) return vec3{ 0,1,0 };
				if constexpr(axis == &vec3::z) return vec3{ 0,0,1 };
			}();

			constexpr auto other1 = []()
			{
				if constexpr (axis == &vec3::x) return &vec3::y;
				if constexpr (axis == &vec3::y) return &vec3::z;
				if constexpr (axis == &vec3::z) return &vec3::x;
			}();

			constexpr auto other2 = []()
			{
				if constexpr (axis == &vec3::x) return &vec3::z;
				if constexpr (axis == &vec3::y) return &vec3::x;
				if constexpr (axis == &vec3::z) return &vec3::y;
			}();

			const auto is_below = l_ray.origin.*axis < r_aabb.min.*axis;

			const real distance = is_below ? r_aabb.min.*axis - l_ray.origin.*axis : r_aabb.max.*axis - l_ray.origin.*axis;

			const real t = distance / l_ray.velocity.*axis;

			if (t < -epsilon) // ray is flying away
			{
				raycast_failure fail;
				fail.nearest_point = l_ray.origin;
				fail.nearest_distance = abs(distance);
				fail.perpendicular = is_below ? -nml : nml;
				return fail;
			}

			// ray is flying towards
			const auto ray_pt = l_ray.get_point_after(t);

			const auto collision_pt = [&]() -> vec3
			{
				auto retval = ray_pt;
				retval.*other1 = std::clamp(ray_pt.*other1, r_aabb.min.*other1, r_aabb.max.*other1);
				retval.*other2 = std::clamp(ray_pt.*other2, r_aabb.min.*other2, r_aabb.max.*other2);
				return retval;
			}();

			if (collision_pt.*other1 == ray_pt.*other1 && collision_pt.*other2 == ray_pt.*other2)
			{
				raycast_success succ;
				succ.point_of_collision    = collision_pt;
				succ.distance_to_collision = distance;
				return succ;
			}

			raycast_failure failure;
			failure.nearest_point = l_ray.origin;
			failure.nearest_distance = abs(distance);
			failure.perpendicular = is_below ? -nml : nml;
			return failure;
		}
	}
}