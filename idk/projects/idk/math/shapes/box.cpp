#include "stdafx.h"
#include "box.h"
#include <math/matrix_decomposition.h>

namespace idk
{
	box& idk::box::operator*=(const mat4& transform)
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