#include "stdafx.h"
#include "collision_result.h"

namespace idk::phys
{
	col_result col_result::operator-() const
	{
		return std::visit([](const auto& obj)->col_result { return -obj; }, result);
	}
	col_result::operator bool() const
	{
		return result.index() == 0;
		//return epsilon_equal(perp_dist, 0) && !(dist == std::numeric_limits<float>::infinity()) && dist >= -epsilon;
	}
	col_failure col_failure::operator-() const
	{
		auto copy = *this;
		copy.separating_axis = -copy.separating_axis;
		return copy;
	}
	col_success col_success::operator-() const
	{
		auto copy = *this;
		copy.normal_of_collision = -copy.normal_of_collision;
		std::swap(copy.penetration_depth_l, copy.penetration_depth_r);
		return copy;
	}
}
