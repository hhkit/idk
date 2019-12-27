#include "stdafx.h"
#include "ritters.h"
#include <ds/span.inl>

namespace idk
{
	sphere ritters(span<vec3> positions)
	{
		IDK_ASSERT_MSG(positions.size() > 1, "Ritters cannot be computed.");

		auto pt_x = positions[0];
		auto pt_y = pt_x;
		auto dist_sq = 0.f;

		for (auto& pos : positions)
		{
			auto new_dist_sq = pos.distance_sq(pt_x);
			if (new_dist_sq > dist_sq)
			{
				pt_y = pos;
				dist_sq = new_dist_sq;
			}
		}
		
		auto pt_z = pt_x;

		for (auto& pos : positions)
		{
			auto new_dist_sq = pos.distance_sq(pt_y);
			if (new_dist_sq > dist_sq)
			{
				pt_z = pos;
				dist_sq = new_dist_sq;
			}
		}
		
		sphere retval{(pt_y + pt_z ) / 2, pt_y.distance(pt_z) / 2};

		for (auto& elem : positions)
			retval.surround(elem);
		return retval;
	}

	sphere ritters(span<const vec3> positions)
	{
		IDK_ASSERT_MSG(positions.size() > 1, "Ritters cannot be computed.");

		auto pt_x = positions[0];
		auto pt_y = pt_x;
		auto dist_sq = 0.f;

		for (auto& pos : positions)
		{
			auto new_dist_sq = pos.distance_sq(pt_x);
			if (new_dist_sq > dist_sq)
			{
				pt_y = pos;
				dist_sq = new_dist_sq;
			}
		}

		auto pt_z = pt_x;

		for (auto& pos : positions)
		{
			auto new_dist_sq = pos.distance_sq(pt_y);
			if (new_dist_sq > dist_sq)
			{
				pt_z = pos;
				dist_sq = new_dist_sq;
			}
		}

		sphere retval{ (pt_y + pt_z) / 2, pt_y.distance(pt_z) / 2 };

		for (auto& elem : positions)
			retval.surround(elem);
		return retval;
	}
}