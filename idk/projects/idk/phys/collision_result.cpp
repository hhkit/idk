#include "stdafx.h"
#include "collision_result.h"
#include <ds/result.inl>

namespace idk::phys
{
	col_result col_result::operator-() const
	{
		return *this ? col_result{ -value() } : col_result{ -error() };
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
		copy.normal = -copy.normal;
		std::swap(copy.centerA, copy.centerB);
		return copy;
	}
}
