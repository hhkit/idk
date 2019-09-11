#pragma once
#include <idk.h>
#include <math/shapes/aabb.h>

namespace idk
{
	struct box
	{
		vec3 position;
		mat3 half_extents;

		aabb bounds() const;

		box& operator*=(const mat4& transform);
		box  operator*(const mat4& matrix) const;
	};
}