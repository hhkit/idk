#pragma once
#include <idk.h>
#include <ds/result.h>
namespace idk::phys
{
	bool epsilon_equal(real lhs, real rhs);

	struct col_success
	{
		vec3 point_of_collision  {};
		vec3 normal_of_collision {}; // normal points towards lhs
		real penetration_depth   {};

		col_success operator-() const;
	};

	struct col_failure
	{
		real perp_dist        { -1.0f }; // default false.
		vec3 separating_axis  {};

		col_failure operator-() const;
	};

	struct col_result
		: public monadic::result<col_success, col_failure>
	{
	private:
		using Base = monadic::result<col_success, col_failure>;
	public:
		using Base::Base;      // use result's ctors
		using Base::operator=; // use result's assignment

		col_result operator-() const;
	};
}