//////////////////////////////////////////////////////////////////////////////////
//@file		collision_capsule.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		4 NOV 2019
//@brief	

/*
Collisions with capsule
*/
//////////////////////////////////////////////////////////////////////////////////


#pragma once
#include <phys/collision_result.h>
#include <math/shapes/capsule.h>
#include <math/shapes/sphere.h>

namespace idk::phys
{
	col_result collide_capsule_sphere_discrete(const capsule& lhs, const sphere& rhs);
}