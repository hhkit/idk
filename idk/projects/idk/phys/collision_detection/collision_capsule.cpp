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

	//col_result collide_capsule_box_discrete(const capsule& lhs, const box& rhs)
	//{
	//	return col_result();
	//}
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