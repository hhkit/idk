#include "pch.h"
#include <phys/raycasts/collision_raycast.h>

namespace idk::phys
{
	bool col_result_expect(const col_result& data, float perp_dist, float dist)
	{
		using namespace idk;
		bool resultv = data ? true :
			epsilon_equal(data.error().perp_dist, dist) && phys::epsilon_equal(data.error().perp_dist, perp_dist);
		return resultv;
	}
	bool IsNearest(ray r, ray line, col_result v)
	{
		using namespace idk;
		bool result = false;
		const auto dist = [&]()
		{
			if (v)
				return v.value().penetration_depth;
			else
				return v.error().perp_dist;
		}();

		if (dist != std::numeric_limits<float>::infinity())
		{
			ray& l = line;
			float t = dist;
			vec3 point = r.get_point(t);
			vec3 disp = point - l.origin;
			vec3 l_disp_to_closest = disp.project_onto(l.velocity);
			vec3 l_closest_point = l.origin + l_disp_to_closest;
	
			vec3 offset_plus = point + r.velocity * 0.1f;
			vec3 offset_minus = point - r.velocity * 0.1f;
			float perp_dist = l_closest_point.distance(point);
			result = (perp_dist < l_closest_point.distance(offset_plus) && perp_dist < l_closest_point.distance(offset_minus)) && phys::epsilon_equal(perp_dist, perp_dist);
		}
		else
		{
			vec3 diff = line.origin - r.origin;
			result = phys::epsilon_equal(r.velocity.dot(line.velocity), r.velocity.length() * line.velocity.length()) && (diff - diff.project_onto(r.velocity)).length() == dist;
		}
		return result;
	}
	bool PD_Test(ray r, ray line)
	{
		using namespace idk;
		auto cresult = phys::collide_ray_line(r, line);
		return IsNearest(r, line, cresult);
	}
}
TEST(StaticCollisionTest, RayLineTest)
{
	using namespace idk;
	using namespace idk::phys;

	vec3 origin = vec3{ 0.0f };
	vec3 xaxis = vec3{ 1.0f,0.0f,0.0f };
	vec3 yaxis = vec3{ 0.0f,1.0f,0.0f };
	vec3 zaxis = vec3{ 0.0f,0.0f,1.0f };
	vec3 r_origin = origin + vec3{ -1,-2,-3 };
	vec3 r_dir = (origin -r_origin ).get_normalized();
	vec3 tmp = r_dir * (r_origin - origin).length();
	//Succeed: with origin.
	EXPECT_TRUE(col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + xaxis,xaxis }), 0, (r_origin - origin).length()));
	EXPECT_TRUE(col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin,xaxis }),0,(r_origin-origin).length()));
	//Succeed: perp_dist = 0, dist = magnitude of r_origin
	EXPECT_TRUE(col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - xaxis,xaxis }), 0, (r_origin - origin).length()));
	EXPECT_TRUE(col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + yaxis,yaxis }), 0, (r_origin - origin).length()));
	EXPECT_TRUE(col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - yaxis,yaxis }), 0, (r_origin - origin).length()));
	EXPECT_TRUE(col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + zaxis,zaxis }), 0, (r_origin - origin).length()));
	EXPECT_TRUE(col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - zaxis,zaxis }), 0, (r_origin - origin).length()));

	//Fail: perp_dist = 2.0f, dist = magnitude of r_origin - origin
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin + 2.0f * yaxis,xaxis }));//, 2.0f, (r_origin - origin).length()));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin - 2.0f * yaxis,xaxis }));//, 2.0f, (r_origin - origin).length()));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin + 2.0f * zaxis,xaxis }));//, 2.0f, (r_origin - origin).length()));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin - 2.0f * zaxis,xaxis }));//, 2.0f, (r_origin - origin).length()));
																				  
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin + 2.0f * xaxis,yaxis }));//, 2.0f, (r_origin - origin).length()));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin - 2.0f * xaxis,yaxis }));//, 2.0f, (r_origin - origin).length()));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin + 2.0f * zaxis,yaxis }));//, 2.0f, (r_origin - origin).length()));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin - 2.0f * zaxis,yaxis }));//, 2.0f, (r_origin - origin).length()));
																				  
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin + 2.0f * xaxis,zaxis }));//, 2.0f, (r_origin - origin).length()));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin - 2.0f * xaxis,zaxis }));//, 2.0f, (r_origin - origin).length()));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin + 2.0f * yaxis,zaxis }));//, 2.0f, (r_origin - origin).length()));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin - 2.0f * yaxis,zaxis }));//, 2.0f, (r_origin - origin).length()));
	//Succeed perp_dist = 0, dist = 0
	EXPECT_TRUE(col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin         , r_dir }), 0.0f, 0.0f));
	EXPECT_TRUE(col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin + r_dir , r_dir }), 0.0f, 0.0f));
	EXPECT_TRUE(col_result_expect(collide_ray_line(ray{ r_origin,r_dir }, ray{ origin - r_dir , r_dir }), 0.0f, 0.0f));
	//Fail perp_dist = 1, dist = infinite
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin + xaxis , r_dir }));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin - xaxis , r_dir }));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin + yaxis , r_dir }));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin - yaxis , r_dir }));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin + zaxis , r_dir }));
	EXPECT_TRUE(PD_Test(ray{ r_origin,r_dir }, ray{ origin - zaxis , r_dir }));
}
