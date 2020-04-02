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
//#include <phys/collision.cpp>
#include <gfx/DebugRenderer.h> //Temp
#include <ds/result.inl>

namespace idk::phys
{
	std::pair<vec3, vec3> GJKAlgorithm(const vector<vec3> points);
	std::pair<vec3, vec3> ClosestPairPointsLineSegmentToLineSegment(const vec3& lineA_1, const vec3& lineA_2, const vec3& lineB_1, const vec3& lineB_2);


	col_result collide_capsule_capsule_discrete(const capsule& lhs, const capsule& rhs)
	{

		const auto line_points_A = lhs.get_line_points(); //A1,A0
		const auto line_points_B = rhs.get_line_points(); //B1,B0

		auto closestPairPts = ClosestPairPointsLineSegmentToLineSegment(line_points_A.first, line_points_A.second, line_points_B.first, line_points_B.second);

		Core::GetSystem<DebugRenderer>().Draw(closestPairPts.first, closestPairPts.second, color{ 0,0,1,1 });
		const sphere makeshiftSphereA{ closestPairPts.first,lhs.radius };
		const sphere makeshiftSphereB{ closestPairPts.second,lhs.radius };

		auto collisionRes = collide_sphere_sphere_discrete(makeshiftSphereA, makeshiftSphereB);
		if (collisionRes) {
			Core::GetSystem<DebugRenderer>().Draw(makeshiftSphereA, color{ 0,1,0,1 });
			Core::GetSystem<DebugRenderer>().Draw(makeshiftSphereB, color{ 0,1,0,1 });
		}
		return collisionRes;

	}

}