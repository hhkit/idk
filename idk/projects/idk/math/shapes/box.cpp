#include "stdafx.h"
#include "box.h"
#include <math/matrix_decomposition.h>

namespace idk
{
	aabb box::bounds() const
	{
		vec3 hextents{};
		for (auto [cvec, length]: zip(axes, half_extents()))
		{
			auto col_vec = cvec;
			for (auto& elem : col_vec)
				elem = abs(elem);
			hextents += col_vec * length;
		}

		return aabb{ center - hextents, center + hextents };
	}
	vec3 box::half_extents() const
	{
		return extents / 2;
	}
	array<vec3, 8> box::points() const
	{
		const auto hextents = half_extents();
		return array<vec3, 8>{
			center - hextents[0] * axes[0] - hextents[1] * axes[1] - hextents[2] * axes[2],
			center - hextents[0] * axes[0] - hextents[1] * axes[1] + hextents[2] * axes[2],
			center - hextents[0] * axes[0] + hextents[1] * axes[1] - hextents[2] * axes[2],
			center - hextents[0] * axes[0] + hextents[1] * axes[1] + hextents[2] * axes[2],
			center + hextents[0] * axes[0] - hextents[1] * axes[1] - hextents[2] * axes[2],
			center + hextents[0] * axes[0] - hextents[1] * axes[1] + hextents[2] * axes[2],
			center + hextents[0] * axes[0] + hextents[1] * axes[1] - hextents[2] * axes[2],
			center + hextents[0] * axes[0] + hextents[1] * axes[1] + hextents[2] * axes[2],
		};
	}
	box& box::operator*=(const mat4& transform)
	{
		auto tfm = transform * mat4{ scale(extents) * axes };
		extents  = vec3{ tfm[0].length(), tfm[1].length(), tfm[2].length() };
		axes     = orthonormalize(mat3{ tfm });
		center  += tfm[3].xyz;
		return *this;
	}
	box box::operator*(const mat4& matrix) const
	{
		auto copy = *this;
		return copy *= matrix;
	}
}