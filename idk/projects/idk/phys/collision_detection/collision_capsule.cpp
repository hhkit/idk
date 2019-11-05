//////////////////////////////////////////////////////////////////////////////////
//@file		collision_capsule.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		4 NOV 2019
//@brief	

/*
Collisions with capsule. Currently has debug renders
*/
//////////////////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "collision_capsule.h"
#include "collision_sphere.h"
#include <gfx/DebugRenderer.h> //Temp

namespace idk::phys
{

	col_result collide_capsule_box_discrete(const capsule& lhs, const box& rhs)
	{
		//Use OOB(Object Oriented Bounding)
		//printf("BoxCollisionTest!\n");
		return col_failure();
	}

	col_result collide_capsule_capsule_discrete(const capsule& lhs, const capsule& rhs)
	{

		const auto line_points_A = lhs.get_line_points(); //A1,A0
		const auto line_points_B = rhs.get_line_points(); //B1,B0

		const vec3 A_vector = line_points_A.first - line_points_A.second;
		const vec3 B_vector = line_points_B.first - line_points_B.second;
		const vec3 A_unit_vector = A_vector.get_normalized();
		const vec3 B_unit_vector = B_vector.get_normalized();
		const vec3 cross_vector = A_unit_vector.cross(B_unit_vector);
		const auto denominator = cross_vector.length_sq();

		vec3 p_A{};
		vec3 p_B{};
		if (epsilon_equal(denominator, 0)) { //Check if it is parallel, if it is, then check if it is above or below
			const auto dot_1 = A_unit_vector.dot(line_points_B.second - line_points_A.second);
			const auto dot_2 = A_unit_vector.dot(line_points_B.first  - line_points_A.second);


			if (dot_1 <= 0 && dot_2 <= 0) {
				if (abs(dot_1) < abs(dot_2)) { 
					p_A = line_points_A.second;
					p_B = line_points_B.second;
				}
				else {
					p_A = line_points_A.second;
					p_B = line_points_B.first;
				}
			}
			else if (dot_1 >= A_vector.length() && dot_2 >= A_vector.length()) {
				if (abs(dot_1) < abs(dot_2)) {
					p_A = line_points_A.first;
					p_B = line_points_B.second;
				}
				else {
					p_A = line_points_A.first;
					p_B = line_points_B.first;
				}
			}
			else {	//Parallel and both overlap

				const vec3 center_vector = lhs.center - rhs.center;
				float length_at_midway = center_vector.dot(rhs.dir) * 0.5f;
				p_B = rhs.center + rhs.dir * length_at_midway;
				p_A = lhs.center - rhs.dir * length_at_midway;
			}


		}
		else {	//Not parallel, they skew
			const vec3 connecting_vector = line_points_B.second- line_points_A.second;
			auto determinant_A = mat3{ connecting_vector ,B_unit_vector ,cross_vector }.determinant();
			auto determinant_B = mat3{ connecting_vector ,A_unit_vector ,cross_vector }.determinant();

			auto projected_length_A = determinant_A / denominator;
			auto projected_length_B = determinant_B / denominator;

			p_A = line_points_A.second + A_unit_vector * projected_length_A;	//Closest points on 2 infinite lines
			p_B = line_points_B.second + B_unit_vector * projected_length_B;	//Closest points on 2 infinite lines

			//Clamp to its edges
			if (projected_length_A < 0)
				p_A = line_points_A.second;
			else if (projected_length_A > A_vector.length())
				p_A = line_points_A.first;

			if (projected_length_B < 0)
				p_B = line_points_B.second;
			else if (projected_length_B > B_vector.length())
				p_B = line_points_B.first;


			float final_length{};
			if (projected_length_A < 0 || projected_length_A > A_vector.length())
			{
				final_length = dot(B_unit_vector, (p_A - line_points_B.second));
				if (final_length < 0)
					final_length = 0;
				else if (final_length > B_vector.length())
					final_length = B_vector.length();
				p_B = line_points_B.first + (B_unit_vector * final_length);
			}
			
			if (projected_length_B < 0 || projected_length_B > B_vector.length())
			{
				final_length = dot(A_unit_vector, (p_B - line_points_A.second));
				if (final_length < 0)
					final_length = 0;
				else if (final_length > A_vector.length())
					final_length = A_vector.length();
				p_A = line_points_A.first + (A_unit_vector * final_length);
			}

			

		}

		Core::GetSystem<DebugRenderer>().Draw(p_A, p_B, color{ 0,0,1,1 });
		const sphere makeshiftSphereA{ p_A,lhs.radius };
		const sphere makeshiftSphereB{ p_B,lhs.radius };

		auto collisionRes = collide_sphere_sphere_discrete(makeshiftSphereA, makeshiftSphereB);
		try {
			collisionRes.value();
			Core::GetSystem<DebugRenderer>().Draw(makeshiftSphereA, color{ 0,1,0,1 });
			Core::GetSystem<DebugRenderer>().Draw(makeshiftSphereB, color{ 0,1,0,1 });

		}
		catch (...) {
		}
		return collisionRes;

	}


	col_result collide_capsule_sphere_discrete(const capsule& lhs, const sphere& rhs)
	{
		//printf("Center: %.2f,%.2f,%.2f\n", lhs.center.x, lhs.center.y, lhs.center.z);
		//printf("Dir: %.2f,%.2f,%.2f\n", lhs.dir.x, lhs.dir.y, lhs.dir.z);
		//Find Closest point from the sphere
		if (lhs.is_sphere()) {
			const sphere makeshiftSphere{ lhs.center,lhs.radius};

			auto collisionRes = collide_sphere_sphere_discrete(rhs, makeshiftSphere);
			try {
				collisionRes.value();
				Core::GetSystem<DebugRenderer>().Draw(makeshiftSphere, color{ 0,1,0,1 });
				Core::GetSystem<DebugRenderer>().Draw(lhs.center, lhs.center + collisionRes.value().normal_of_collision, color{ 0,0,1,1 });

			}
			catch (...) {
			}
			return collisionRes;

		}
		//At this point, the capsule is actually a capsule and not a sphere

		const vec3 top_line_point = lhs.center + (lhs.dir.get_normalized() * ((lhs.height * 0.5f) - lhs.radius)); //Capsule is just a line with a radius all around it.
		const vec3 bot_line_point = lhs.center - (lhs.dir.get_normalized() * ((lhs.height * 0.5f) - lhs.radius)); 
		const vec3 line_vector = top_line_point- bot_line_point; //Top to bot
		const vec3 sphere_to_top_vector = rhs.center - top_line_point;	//The hypotenuse of the imaginary triangle
		const vec3 sphere_to_bot_vector = rhs.center - bot_line_point;	//The hypotenuse of the imaginary triangle


		const float dot_val_top = sphere_to_top_vector.dot(-line_vector); //Positive denotes it is within line boundary
		const float dot_val_bot = sphere_to_bot_vector.dot(line_vector); //Positive denotes it is within line boundary
		const sphere makeshiftSphereA{ top_line_point,lhs.radius * 1.0f };
		const sphere makeshiftSphereB{ bot_line_point,lhs.radius * 1.0f };


		if		(dot_val_top > 0 && dot_val_bot <= 0) {		//The sphere is located somewhere below the cap of the capsule
			const sphere makeshiftSphere{ bot_line_point,lhs.radius };
			auto collisionRes = collide_sphere_sphere_discrete(rhs, makeshiftSphere);
			try {
				collisionRes.value();
				//printf("Bot Cap Collision Success!\n");
				Core::GetSystem<DebugRenderer>().Draw(makeshiftSphere, color{ 0,1,0,1 });
				Core::GetSystem<DebugRenderer>().Draw(bot_line_point, bot_line_point+ collisionRes.value().normal_of_collision, color{ 0,0,1,1 });
			}
			catch (...) {
				//printf("Bot Cap Collision Failed!\n");
			}
			return collisionRes;
		}
		else if (dot_val_bot > 0 && dot_val_top <= 0) {		//The sphere is located somewhere above the cap of the capsule
			const sphere makeshiftSphere{ top_line_point,lhs.radius };
			auto collisionRes = collide_sphere_sphere_discrete(rhs, makeshiftSphere);
			try {
				collisionRes.value();
				//printf("Top Cap Collision Success!\n");
				Core::GetSystem<DebugRenderer>().Draw(makeshiftSphere, color{ 0,1,0,1 });
				Core::GetSystem<DebugRenderer>().Draw(top_line_point, top_line_point + collisionRes.value().normal_of_collision, color{ 0,0,1,1 });

			}
			catch (...) {
				//printf("Top Cap Collision Failed!\n");
			}
			return collisionRes;
		}
		else {												//The sphere is located somewhere beside the capsule
			//Find closest normal, then
			//Dot product onto the normalized vector line to get magnitude of line_vector

			const float magnitude = sphere_to_bot_vector.dot(line_vector.get_normalized());
			const vec3 normal_point = bot_line_point + line_vector.get_normalized() * magnitude;

			const sphere makeshiftSphere{ normal_point,lhs.radius };
			auto collisionRes = collide_sphere_sphere_discrete(rhs, makeshiftSphere);
			try {
				collisionRes.value();
				//printf("Mid Cap Collision Success!\n");
				Core::GetSystem<DebugRenderer>().Draw(makeshiftSphere, color{ 0,1,0,1 });
				Core::GetSystem<DebugRenderer>().Draw(normal_point, normal_point + collisionRes.value().normal_of_collision, color{ 0,0,1,1 });

			}
			catch (...) {
				//printf("Mid Cap Collision Failed!\n");
			}
			return collisionRes;
		}
	}
}