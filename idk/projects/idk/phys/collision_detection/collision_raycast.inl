#pragma once
#include <phys/collision_result.h>
namespace idk::phys
{
	namespace detail
	{
		template<decltype(&vec3::x) axis>
		col_result collide_ray_aabb_face(vec3 direction, const vec3& disp_to_box, vec3 extents)
		{
			const real t = direction.*axis / (disp_to_box.*axis - extents.*axis);

			constexpr auto nml = []()
			{
				if constexpr(axis == &vec3::x) return vec3{ 1,0,0 };
				if constexpr(axis == &vec3::y) return vec3{ 0,1,0 };
				if constexpr(axis == &vec3::z) return vec3{ 0,0,1 };
				return vec3{};
			}();

			if (t > -epsilon)
			{
				auto  point_at_edge = disp_to_box - direction * t;
				point_at_edge.*axis = 0;
				point_at_edge = abs(point_at_edge);
				const auto pd_not_inside = PerpDist<axis>(point_at_edge, extents);

				if ((abs(point_at_edge.x) <= extents.x && abs(point_at_edge.y) <= extents.y && abs(point_at_edge.z) <= extents.z))
				{
					col_success succ;
					succ.normal_of_collision = direction.dot(nml) > 0 ? nml : -nml;
					succ.penetration_depth = 0;
					succ.point_of_collision.*axis = point_at_edge.*axis;
					return succ;
				}
				else
				{
					col_failure fail;
					fail.perp_dist = pd_not_inside;
					fail.separating_axis = nml;
					return fail;
				}
			}

			col_failure fail;
			fail.perp_dist = abs(t);
			fail.separating_axis = disp_to_box.dot(nml) > 0 ? nml : -nml;
			return fail;
		}
	}
}