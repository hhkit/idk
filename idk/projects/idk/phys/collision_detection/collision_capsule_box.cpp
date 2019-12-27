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
#include "collision_capsule_box.h"
#include "collision_box_sphere.h"
#include <gfx/DebugRenderer.h> //Temp
#include <ds/result.inl>

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

			collisionResult = collide_box_sphere_discrete(rhs, makeshiftSphere);
			if (collisionResult)
				Core::GetSystem<DebugRenderer>().Draw(makeshiftSphere, color{ 0,1,0,1 });
		}

		



		return collisionResult;
	}

}