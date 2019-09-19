#include "stdafx.h"
#include <phys/raycasts/collision_raycast.h>
namespace idk::phys
{
	bool epsilon_equal(real lhs, real rhs)
	{
		//Gotta handle infinity
		return lhs == rhs || abs(abs(rhs) - abs(lhs)) <= epsilon;
	}
}