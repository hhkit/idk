#include "stdafx.h"
#include "box.h"
#include <math/matrix_decomposition.h>

namespace idk
{
	aabb box::bounds() const
	{
		vec3 extents{};
		for (auto [cvec, length]: zip(axes, extents))
		{
			auto col_vec = cvec;
			for (auto& elem : col_vec)
				elem = abs(elem);
			extents += col_vec * length;
		}
		extents /= 2;

		return aabb{ center - extents, center + extents };
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