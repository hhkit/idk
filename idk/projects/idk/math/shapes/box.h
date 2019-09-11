#pragma once
#include <idk.h>

namespace idk
{
	struct box
	{
		vec3 position;
		mat3 half_extents;

		box& operator*=(const mat4& transform);
		box  operator*(const mat4& matrix) const;
	};
}