#include "stdafx.h"
#include "collision_obb.h"

namespace idk::phys
{
	col_result collide_box_box_static(const box& lhs, const box& rhs)
	{
		col_result retval;

		const auto& c0 = lhs.center;
		const auto& e0 = lhs.extents[0];

		constexpr auto cutoff = real{ 1 } - epsilon;
		bool exists_parallel_pair = false;

		auto dist = rhs.center - lhs.center;

		mat3 dotLR;          // dotLR[i][j] = lhs[i].dot(rhs[j]);
		mat3 absDotLR;       // abs(dotLR)
		real dotDistL[3];    // dot(dist, lhs[0]);
		real rL, rR, r;
		real rLR;

		real shortest_dist;
		vec3 normal_of_col;

		// test for separation on lhs's axes
		for (auto index : range<3>())
		{
			dotLR[index] = vec3{
				lhs.axes[index].dot(rhs.axes[0]),
				lhs.axes[index].dot(rhs.axes[1]),
				lhs.axes[index].dot(rhs.axes[2])
			};

			for (auto [avec, vec] : zip(absDotLR[index], dotLR[index]))
			{
				avec = abs(vec);
				if (avec > cutoff)
					exists_parallel_pair = true;
			}

			dotDistL[index] = dist.dot(lhs.axes[index]);
			r = abs(dotDistL[index]);	                   // project distance onto axis
			rL = rhs.extents.dot(absDotLR[index]);         // project right extents onto axis
			rLR = lhs.extents[index] + rL;                 // add to the left extents
			if (r > rLR)
			{
				retval.intersect = false;
				retval.separating_axis = lhs.axes[index];
				return retval;
			}
		}

		// test for separation on rhs's axes
		for (auto index : range<3>())
		{
			r = abs(dist.dot(rhs.axes[index]));
			// dot with the transpose
			rL = lhs.extents[0] * absDotLR[0][index] + lhs.extents[1] * absDotLR[1][index] + lhs.extents[2] * absDotLR[2][index];
			rLR = rL + rhs.extents[index];
			if (r > rLR)
			{
				retval.intersect = false;
				retval.separating_axis = rhs.axes[index];
				return retval;
			}
		}

		if (exists_parallel_pair)
		{
			retval.intersect = true;
			return retval;
		}
	}
}