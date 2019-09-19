#include "stdafx.h"
#include "collision_sphere.h"

namespace idk::phys
{
	col_result collide_sphere_sphere_discrete(const sphere& lhs, const sphere& rhs)
	{
		if (lhs.overlaps(rhs))
		{
			col_success succ;
			succ.normal_of_collision = (lhs.center - rhs.center).normalize();
			succ.point_of_collision  = (rhs.center + lhs.center) / 2;
			succ.penetration_depth   = abs(lhs.radius + rhs.radius - lhs.center.distance(rhs.center));
			return succ;
		}
		else
		{
			col_failure fail;
			auto diff = (lhs.center - rhs.center);
			auto diff_length = diff.length();
			fail.perp_dist = diff_length - lhs.radius - rhs.radius;
			fail.separating_axis = diff / diff_length;
			return fail;
		}
	}
}
