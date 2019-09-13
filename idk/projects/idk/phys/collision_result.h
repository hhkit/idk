#pragma once
#include <idk.h>

namespace idk::phys
{
	bool epsilon_equal(real lhs, real rhs);

	struct col_result
	{
		real perp_dist = -1.0f; // default false.
		real dist = 0.0f;
		vec3 normal_of_collision {};
		vec3 separating_axis     {};
		bool intersect = false;

		explicit operator bool() const;
	};
}