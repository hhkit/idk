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
#include "collision_box_sphere.h"
#include <gfx/DebugRenderer.h> //Temp

namespace idk::phys
{

	col_result collide_capsule_box_discrete(const capsule& lhs, const box& rhs)
	{

		//TRS
		//Make capsule to two points
		//Translate both points to T-1 then R-1 using box's reference.


		//Find Closest point, using two closest points from box from line generated from capsule
		
		/*
		const auto box_points = rhs.points();
		auto capsule_points = lhs.get_line_points();
		vec3 support_point_A{ box_points[0]}; //Closest point to the capsule
		vec3 support_point_B{ box_points[0]};
		float closest_total_distance_from_point { vec3(box_points[0]- capsule_points.first).length_sq() + vec3(box_points[0] - capsule_points.second).length_sq() }; //Total distance from the point to top capsule, and point to bot capsule
		float second_closest_total_distance_from_point { closest_total_distance_from_point }; //Total distance from the point to top capsule, and point to bot capsule

		for (int i = 1; i < box_points.size(); ++i) {
			float check_closest_total_distance_from_point{ vec3(box_points[i] - capsule_points.first).length_sq() + vec3(box_points[i] - capsule_points.second).length_sq() };

			if (check_closest_total_distance_from_point < closest_total_distance_from_point) {
				closest_total_distance_from_point = check_closest_total_distance_from_point;
				support_point_B = support_point_A;
				support_point_A = box_points[i];
			}
			else if (closest_total_distance_from_point < second_closest_total_distance_from_point) {
				second_closest_total_distance_from_point = check_closest_total_distance_from_point;
				support_point_B = box_points[i];
			}
		}
		
		sphere makeshiftsphere{ support_point_A };
		sphere makeshiftsphereB{ support_point_B };
		Core::GetSystem<DebugRenderer>().Draw(makeshiftsphere, color{ 0,1,1,1 }); //cyan
		Core::GetSystem<DebugRenderer>().Draw(makeshiftsphereB, color{ 0,1,0,1 }); //green
		Core::GetSystem<DebugRenderer>().Draw(support_point_A, capsule_points.first, color{ 0,1,1,1 }); //cyan
		Core::GetSystem<DebugRenderer>().Draw(support_point_A, capsule_points.second, color{ 0,1,1,1 }); //cyan
		*/



		//--------------------------------------------------------------
		//Check if capsule and box are perpendicular to each other
		//auto perpendicularity = lhs.dir.dot()


		/*
		const mat3& box_axes = rhs.axes();

		const vec3 axesA = vec3{ box_axes[0][0],box_axes[0][1],box_axes[0][2] };
		const vec3 axesB = vec3{ box_axes[1][0],box_axes[1][1],box_axes[1][2] };
		const vec3 axesC = vec3{ box_axes[2][0],box_axes[2][1],box_axes[2][2] };
		float is_perpendicularA = lhs.dir.dot(axesA);
		float is_perpendicularB = lhs.dir.dot(axesB);
		float is_perpendicularC = lhs.dir.dot(axesC);

		//It takes two axes to determine if it is parallel to box
		bool is_axis_a = epsilon_equal(is_perpendicularA, 0) || epsilon_equal(is_perpendicularA, 1);	//Check if it is either perpendicular or parallel
		bool is_axis_B = epsilon_equal(is_perpendicularB, 0) || epsilon_equal(is_perpendicularB, 1);	//Check if it is either perpendicular or parallel


		if (is_axis_a && is_axis_B) {
			auto& corner_origin = box_points[0];
			const vec3 vector_A = box_points[1] - corner_origin;  //Either parallel or perpendicular. Only two are perpendicular!
			const vec3 vector_B = box_points[2] - corner_origin;  //Either parallel or perpendicular. Only two are perpendicular!
			const vec3 vector_C = box_points[4] - corner_origin;  //Either parallel or perpendicular. Only two are perpendicular!


			Core::GetSystem<DebugRenderer>().Draw(corner_origin, corner_origin + vector_A, color{ 0,1,0,1 }); //green
			Core::GetSystem<DebugRenderer>().Draw(corner_origin, corner_origin + vector_B, color{ 0,1,0,1 }); //green
			Core::GetSystem<DebugRenderer>().Draw(corner_origin, corner_origin + vector_C, color{ 0,1,0,1 }); //green


		}
		else { //Angled collision. THIS IS CHEATED



		}
		*/


		//This only checks the top and bottom cap of the capsule. DOES NOT CHECK THE BODY! So make sure the box is bigger than the capsule!
		const auto pair_points = lhs.get_line_points();
		sphere makeshiftSphere{ pair_points.second, lhs.radius }; //bottom

		auto collisionResult = collide_box_sphere_discrete(rhs, makeshiftSphere);
		if (collisionResult) {
			Core::GetSystem<DebugRenderer>().Draw(makeshiftSphere, color{ 0,1,0,1 });

		}
		else {
			makeshiftSphere = { pair_points.first, lhs.radius };

			auto collisionResult = collide_box_sphere_discrete(rhs, makeshiftSphere);
			if (collisionResult)
				Core::GetSystem<DebugRenderer>().Draw(makeshiftSphere, color{ 0,1,0,1 });
		}

		



		return collisionResult;
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
		else {	//Not parallel, they skew TODO FIX! IT DOES NOT WORK
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
		if (collisionRes) {
			Core::GetSystem<DebugRenderer>().Draw(makeshiftSphereA, color{ 0,1,0,1 });
			Core::GetSystem<DebugRenderer>().Draw(makeshiftSphereB, color{ 0,1,0,1 });
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
			if (collisionRes) {

				Core::GetSystem<DebugRenderer>().Draw(makeshiftSphere, color{ 0,1,0,1 });
				Core::GetSystem<DebugRenderer>().Draw(lhs.center, lhs.center + collisionRes.value().normal_of_collision, color{ 0,0,1,1 });

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
			if (collisionRes) {
				//printf("Bot Cap Collision Success!\n");
				Core::GetSystem<DebugRenderer>().Draw(makeshiftSphere, color{ 0,1,0,1 });
				Core::GetSystem<DebugRenderer>().Draw(bot_line_point, bot_line_point+ collisionRes.value().normal_of_collision, color{ 0,0,1,1 });
			}
			return collisionRes;
		}
		else if (dot_val_bot > 0 && dot_val_top <= 0) {		//The sphere is located somewhere above the cap of the capsule
			const sphere makeshiftSphere{ top_line_point,lhs.radius };
			auto collisionRes = collide_sphere_sphere_discrete(rhs, makeshiftSphere);
			if (collisionRes) {
				//printf("Top Cap Collision Success!\n");
				Core::GetSystem<DebugRenderer>().Draw(makeshiftSphere, color{ 0,1,0,1 });
				Core::GetSystem<DebugRenderer>().Draw(top_line_point, top_line_point + collisionRes.value().normal_of_collision, color{ 0,0,1,1 });

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
			if (collisionRes) {
				//printf("Mid Cap Collision Success!\n");
				Core::GetSystem<DebugRenderer>().Draw(makeshiftSphere, color{ 0,1,0,1 });
				Core::GetSystem<DebugRenderer>().Draw(normal_point, normal_point + collisionRes.value().normal_of_collision, color{ 0,0,1,1 });
			}
			return collisionRes;
		}
	}
}