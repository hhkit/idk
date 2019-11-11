#include "stdafx.h"
#include "collision_box.h"

namespace idk::phys
{
	col_result collide_box_box_discrete(const box& lhs, const box& rhs)
	{
		auto disp = rhs.center - lhs.center;

		// test on lhs
		struct res { real min, max; };
		constexpr auto find_minmax = [](const vec3& n, const auto& pts) -> res
		{
			real lmin = std::numeric_limits<float>::max();
			real lmax = std::numeric_limits<float>::lowest();
			for (auto& pt : pts)
			{
				auto dot = pt.dot(n);
				lmin = std::min(lmin, dot);
				lmax = std::max(lmax, dot);
			}
			return { lmin, lmax };
		};

		vec3 normal_collision;
		auto max_pen = std::numeric_limits<float>::max();

		const auto lpoints = lhs.points();
		const auto rpoints = rhs.points();

		const auto collide = [&](span<const vec3> axes) -> opt<col_failure>
		{
			for (auto& n : axes)
			{
				if (n.length_sq() < epsilon)
					continue;

				auto [lmin, lmax] = find_minmax(n, lpoints);
				auto [rmin, rmax] = find_minmax(n, rpoints);

				if (lmin > rmax || rmin > lmax)
					return col_failure{ {}, n };
				
				auto new_penetration = std::min(rmax - lmin, lmax - rmin);
				if (new_penetration < max_pen)
				{
					max_pen = new_penetration;
					normal_collision = n;
				}
			}
			return std::nullopt;
		};

		const auto l_axes = lhs.axes();
		const auto r_axes = rhs.axes();
		if (auto res = collide(span<const vec3>{l_axes.begin(), l_axes.end()})) // if fail
			return *res;

		if (auto res = collide(span<const vec3>{r_axes.begin(), r_axes.end()})) // if fail
			return *res;

		array<vec3, 9> cross_norms
		{
			l_axes[0].cross(r_axes[0]).normalize(),
			l_axes[0].cross(r_axes[1]).normalize(),
			l_axes[0].cross(r_axes[2]).normalize(),
			l_axes[1].cross(r_axes[0]).normalize(),
			l_axes[1].cross(r_axes[1]).normalize(),
			l_axes[1].cross(r_axes[2]).normalize(),
			l_axes[2].cross(r_axes[0]).normalize(),
			l_axes[2].cross(r_axes[1]).normalize(),
			l_axes[2].cross(r_axes[2]).normalize(),
		};

		if (auto res = collide(span<const vec3>{cross_norms}))
			return *res;
		else
		{
			col_success result;
			result.normal_of_collision = (rhs.center - lhs.center).dot(normal_collision) > 0 ? -normal_collision : normal_collision;
			result.penetration_depth = max_pen;
			result.point_of_collision; // todo
			return result;
		}
	}
}