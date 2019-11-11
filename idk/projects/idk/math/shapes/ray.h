#pragma once
#include <idk.h>

namespace idk
{
	struct ray
	{
		vec3 origin;
		vec3 velocity;

		vec3 direction() const;

		// returns a point at distance units along the ray
		vec3 get_point(real distance) const;
		vec3 get_point_after(real t) const;

	};
}