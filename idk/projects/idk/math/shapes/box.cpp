#include "stdafx.h"
#include "box.h"
#include <math/matrix_decomposition.h>

namespace idk
{
	aabb box::bounds() const
	{
		vec3 extents{};
		for (auto col_vec: half_extents)
		{
			for (auto& elem : col_vec)
				elem = abs(elem);
			extents += col_vec;
		}
		extents /= 2;

		return aabb{ position - extents, position + extents };
	}
	box& box::operator*=(const mat4& transform)
	{
		auto tfm = transform * mat4{ half_extents };
		half_extents = orthonormalize(mat3{ tfm });
		position += tfm[3].xyz;
		return *this;
	}
	box box::operator*(const mat4& matrix) const
	{
		auto copy = *this;
		return copy *= matrix;
	}
}