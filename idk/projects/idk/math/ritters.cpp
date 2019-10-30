#include "stdafx.h"
#include "ritters.h"

namespace idk
{
	sphere ritters(span<vec3> positions)
	{
		IDK_ASSERT(positions.size() > 1, "Ritters cannot be computed.");

		//sphere ret_val;
		//ret_val.center = positions[0] - positions[1];
		//ret_val.radius = ret_val.center.length_sq();
		//ret_val.center /= 2.0f;

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
		/*
		// First pass: compute the best guess for center
		for (auto& pos : positions)
		{
			const float dist_sq = pos.distance_sq(positions[0]);
			if (dist_sq > ret_val.radius)
			{
				ret_val.radius = dist_sq;
				ret_val.center = (positions[0] - pos) / 2.0f;
			}
		}

		for (auto& pos : positions)
		{
			vec3 pos_to_center = pos - ret_val.center;
			const float dist_sq = pos_to_center.length_sq();
			if (dist_sq > ret_val.radius)
			{
				ret_val.radius = dist_sq;

				// Computing new center
				const float dist = sqrt(dist_sq);
				pos_to_center /= dist;
				ret_val.center = pos + pos_to_center * dist;
			}
		}

		ret_val.radius = sqrt(ret_val.radius);
		*/
		sphere retval{(pt_y + pt_z ) / 2, pt_y.distance(pt_z) / 2};

		for (auto& elem : positions)
			retval.surround(elem);
		return retval;
	}
}