#pragma once
#include <idk.h>
#include <math/shapes/aabb.h>

namespace idk
{
	struct box
	{
		vec3 center {};
		vec3 extents{1};	// full extents
		quat rotation {};

		mat3           axes() const;
		aabb           bounds() const;
		vec3           half_extents() const;
		array<vec3, 8> points() const;

		box& operator*=(const mat4& transform);
		box  operator*(const mat4& matrix) const;

	};
}