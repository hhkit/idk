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

			succ.normal = (rhs.center - lhs.center);
			const float normal_len = succ.normal.length();
			succ.normal /= normal_len;
			succ.contacts[0].position = rhs.center - succ.normal * rhs.radius;
			succ.contacts[0].penetration = (normal_len - (rhs.radius + lhs.radius)) * 0.5f;
			// LOG("%f", succ.contacts[0].penetration);
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
