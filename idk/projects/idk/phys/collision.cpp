#include "stdafx.h"
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
	bool epsilon_equal(float lhs, float rhs)
	{
		//Gotta handle infinity
		return lhs == rhs || abs(abs(rhs) - abs(lhs)) <= epsilon;
	}


	//Assumes lhs and rhs are normalized.
	col_result collide_ray_line(const ray& lhs, const ray& line)
	{
		auto& rhs = line;
		col_result result{};
		//construct a plane around lhs
		vec3 normal = lhs.direction.cross(rhs.direction);
		vec3 disp_to_rhs = rhs.origin - lhs.origin;
		if (epsilon_equal(normal.length_sq(), 0))
		{
			//parallel directions therfore, same plane
			result.perp_dist = (disp_to_rhs - disp_to_rhs.project_onto(lhs.direction)).length();
			result.dist = (epsilon_equal(result.perp_dist, 0)) ? 0.0f : std::numeric_limits<float>::infinity();
		}
		else
		{
			result.perp_dist = disp_to_rhs.dot(normal.get_normalized());
			if (!epsilon_equal(result.perp_dist, 0))
			{
				//rays are not on the same plane.

				//Remove the perpendicular component
				disp_to_rhs = disp_to_rhs - disp_to_rhs.project_onto(normal);
			}
			//rays are on the same plane
			vec3 nml_disp = (disp_to_rhs - disp_to_rhs.project_onto(rhs.direction));
			float t = nml_disp.length() / lhs.direction.dot(nml_disp.get_normalized());
			vec3 tmp = lhs.direction * t;
			result.dist = t;
		}
		return result;
	}

	col_result collide_ray_aabb(const ray& lhs, const aabb& bb)
	{
		col_result result{};
		auto disp_to_box = (bb.center() - lhs.origin);
		auto extents = bb.extents();
		result =  collide_ray_aabb_face<&vec3::x>(lhs.direction, disp_to_box, extents);
		auto tmp = collide_ray_aabb_face<&vec3::y>(lhs.direction, disp_to_box, extents);
		result = (tmp.dist >= 0 && tmp.dist < result.dist && tmp.perp_dist <= result.perp_dist) ? tmp : result;
		tmp = collide_ray_aabb_face<&vec3::z>(lhs.direction, disp_to_box, extents);
		result = (tmp.dist >= 0 &&tmp.dist < result.dist && tmp.perp_dist <= result.perp_dist) ? tmp : result;
		return result;
	}


}