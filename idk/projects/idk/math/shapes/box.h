#pragma once
#include <idk.h>
#include <math/shapes/aabb.h>

namespace idk
{
	struct box
	{
		vec3 center;
		vec3 extents;	// full extents
		mat3 axes;

		aabb bounds() const;

		box& operator*=(const mat4& transform);
		box  operator*(const mat4& matrix) const;
	};
}