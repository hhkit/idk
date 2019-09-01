#pragma once
#include <idk.h>
#include <shape/ray.h>
#include <shape/aabb.h>
#include <shape/capsule.h>
#include <shape/sphere.h>
namespace idk
{
	bool epsilon_equal(float lhs,float rhs)
	{
		//Gotta handle infinity
		return lhs == rhs || abs(abs(rhs) - abs(lhs)) <= epsilon;
	}

	struct col_result
	{
		float perp_dist=-1.0f;//default false.
		float dist     = 0.0f;
		operator bool()const { return epsilon_equal(perp_dist, 0)&& !(dist==std::numeric_limits<float>::infinity()) && dist >=-epsilon ; }
	};

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
			float t = lhs.direction.dot(nml_disp);
			result.dist = t;
		}
		return result;
	}
	template<typename T>
	struct abc
	{
		float x, y, z;
	};

	struct Vec3 : vec3 {};

	template<decltype(&vec3::x) >
	float PerpDist(vec3 point_at_edge, vec3 extents);

	template<>
	float PerpDist<(&vec3::x)>(vec3 point_at_edge, vec3 extents)
	{
		vec3 disp_from_corner = (point_at_edge - extents);
		
		return (disp_from_corner.y < 0) ? disp_from_corner.z : ((disp_from_corner.z < 0) ? disp_from_corner.y : disp_from_corner.length());
	}

	template<>
	float PerpDist<(&vec3::y)>(vec3 point_at_edge, vec3 extents)
	{
		vec3 disp_from_corner = (point_at_edge - extents);

		return (disp_from_corner.x < 0) ? disp_from_corner.z : ((disp_from_corner.z < 0) ? disp_from_corner.x : disp_from_corner.length());
	}

	template<>
	float PerpDist<(&vec3::z)>(vec3 point_at_edge, vec3 extents)
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
	col_result collide_ray_aabb(const ray& lhs, const aabb& bb);

	bool col_result_expect(const col_result& data, float perp_dist, float dist)
	{
		return epsilon_equal(data.dist, dist) && epsilon_equal(data.perp_dist, perp_dist);
	}
	void tc_ray_line()
	{
		vec3 origin = vec3{ 0.0f };
		vec3 xaxis = vec3{ 1.0f,0.0f,0.0f };
		vec3 yaxis = vec3{ 0.0f,1.0f,0.0f };
		vec3 zaxis = vec3{ 0.0f,0.0f,1.0f };
		vec3 r_origin = origin+vec3{ -1,-2,-3 };
		vec3 r_dir = -(r_origin-origin).get_normalized();
		//Succeed: with origin.
		auto result = collide_ray_line(ray{ r_origin,r_dir }, ray{ origin,xaxis });
		//Succeed: perp_dist = 0, dist = magnitude of r_origin
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + xaxis,xaxis }),0,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - xaxis,xaxis }),0,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + yaxis,yaxis }),0,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - yaxis,yaxis }),0,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + zaxis,zaxis }),0,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - zaxis,zaxis }),0,(r_origin-origin).length());
		//Fail: perp_dist = 2.0f, dist = magnitude of r_origin - origin
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + 2.0f * yaxis,xaxis }),2.0f,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - 2.0f * yaxis,xaxis }),2.0f,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + 2.0f * zaxis,xaxis }),2.0f,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - 2.0f * zaxis,xaxis }),2.0f,(r_origin-origin).length());

		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + 2.0f * xaxis,yaxis }),2.0f,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - 2.0f * xaxis,yaxis }),2.0f,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + 2.0f * zaxis,yaxis }),2.0f,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - 2.0f * zaxis,yaxis }),2.0f,(r_origin-origin).length());

		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + 2.0f * xaxis,zaxis }),2.0f,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - 2.0f * xaxis,zaxis }),2.0f,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + 2.0f * yaxis,zaxis }),2.0f,(r_origin-origin).length());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - 2.0f * yaxis,zaxis }),2.0f,(r_origin-origin).length());
		//Succeed perp_dist = 0, dist = 0
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin         , r_dir }),0.0f,0.0f);
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + r_dir , r_dir }),0.0f,0.0f);
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - r_dir , r_dir }),0.0f,0.0f);
		//Fail perp_dist = 1, dist = infinite
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + xaxis , r_dir }),1.0f,std::numeric_limits<float>::infinity());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - xaxis , r_dir }),1.0f,std::numeric_limits<float>::infinity());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + yaxis , r_dir }),1.0f,std::numeric_limits<float>::infinity());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - yaxis , r_dir }),1.0f,std::numeric_limits<float>::infinity());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + zaxis , r_dir }),1.0f,std::numeric_limits<float>::infinity());
		col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - zaxis , r_dir }),1.0f,std::numeric_limits<float>::infinity());
	}
}