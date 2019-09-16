#pragma once
#include <idk.h>

namespace idk::phys
{
	bool epsilon_equal(real lhs, real rhs);

	struct col_success
	{
		vec3 point_of_collision  {};
		vec3 normal_of_collision {}; // normal points towards lhs
		real penetration_depth_l {};
		real penetration_depth_r {};

		col_success operator-() const;
	};

	struct col_failure
	{
		real perp_dist        { -1.0f }; // default false.
		vec3 separating_axis  {};

		col_failure operator-() const;
	};

	struct col_result
	{
		col_result(const col_success& res) : result{ res } {}
		col_result(const col_failure& res) : result{ res } {}

		const col_success& success() const { return std::get<col_success>(result); }
		const col_failure& failure() const { return std::get<col_failure>(result); }

		col_result operator-() const;
		explicit operator bool() const;
	private:
		std::variant<col_success, col_failure> result;
	};
}