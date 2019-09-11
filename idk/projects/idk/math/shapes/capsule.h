#pragma once
#include <idk.h>

namespace idk
{
	struct capsule
	{
		real radius;
		real height;
		vec3 center;
		vec3 dir; // taken to be "forward" or "z-axis"

		capsule& operator*=(const mat4& tfm);
		capsule  operator*(const mat4& tfm) const;
	};
}