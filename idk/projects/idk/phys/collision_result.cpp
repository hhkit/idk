#include "stdafx.h"
#include "collision_result.h"

namespace idk::phys
{
	col_result::operator bool() const
	{
		return result.index() == 0;
		//return epsilon_equal(perp_dist, 0) && !(dist == std::numeric_limits<float>::infinity()) && dist >= -epsilon;
	}
}
