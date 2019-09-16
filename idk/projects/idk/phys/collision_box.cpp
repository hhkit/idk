#include "stdafx.h"
#include "collision_box.h"

namespace idk::phys
{
	col_result collide_box_box_discrete(const box& lhs, const box& rhs)
	{
		auto disp = rhs.center - lhs.center;
		auto max_pen = std::numeric_limits<float>::min();

		auto letsago = [&](const vec3& disp, const vec3& n)
		{

		};

		array<vec3, 9> cross_norms
		{
			lhs.axes[0].cross(rhs.axes[0]).normalize(),
			lhs.axes[0].cross(rhs.axes[1]).normalize(),
			lhs.axes[0].cross(rhs.axes[2]).normalize(),
			lhs.axes[1].cross(rhs.axes[0]).normalize(),
			lhs.axes[1].cross(rhs.axes[1]).normalize(),
			lhs.axes[1].cross(rhs.axes[2]).normalize(),
			lhs.axes[2].cross(rhs.axes[0]).normalize(),
			lhs.axes[2].cross(rhs.axes[1]).normalize(),
			lhs.axes[2].cross(rhs.axes[2]).normalize(),
		};

		return col_failure{};
	}
}