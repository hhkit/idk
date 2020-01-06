#pragma once
#include <phys/collision_result.h>
#include <math/shapes.h>
#include <ds/result.h>

namespace idk::phys
{
	struct raycast_success
	{
		bool origin_colliding     {};
		vec3 point_of_collision   {};
		real distance_to_collision{};
	};

	struct raycast_failure
	{
		vec3 nearest_point    {};
		real nearest_distance {};
		vec3 perpendicular    {};
	};

	using raycast_result = monadic::result<raycast_success, raycast_failure>;

	namespace detail
	{
		template<decltype(&vec3::x) >
		float PerpDist(vec3 point_at_edge, vec3 extents);

		template<decltype(&vec3::x) axis>
		raycast_result collide_ray_aabb_face(const ray& ray, const vec3& ray_to_box, const aabb& aabb, const vec3& extents);
	}

	//Assumes lhs and rhs are normalized.
	raycast_result collide_ray_halfspace(const ray& lhs, const halfspace& rhs);

	col_result collide_ray_line(const ray& lhs, const ray& line);

	raycast_result collide_ray_aabb(const ray& lhs, const aabb& bb);
	raycast_result collide_ray_box(const ray& lhs, const box& bb);

	raycast_result collide_ray_sphere(const ray& lhs, const sphere& s);

	raycast_result collide_ray_capsule(const ray& lhs, const capsule& c);
}
