#include "stdafx.h"
#include "collision_sphere.h"
#include <phys/collision_rotational.h>
namespace idk::phys
{
	col_result collide_sphere_sphere_discrete(const sphere& lhs, const sphere& rhs)
	{
		if (lhs.overlaps(rhs))
		{
			col_success succ;
			succ.centerA = lhs.center;
			succ.centerB = rhs.center;

			succ.normal = (lhs.center - rhs.center).normalize();
			succ.contacts[0].position = (rhs.center + lhs.center) / 2;
			succ.contacts[0].penetration = abs(lhs.radius + rhs.radius - lhs.center.distance(rhs.center));
			succ.contactCount = 1;
			succ.max_penetration = succ.contacts[0].penetration;
			
			compute_basis(succ.normal, succ.tangentVectors, succ.tangentVectors + 1);
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
