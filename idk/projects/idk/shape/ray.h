#pragma once
#include <idk.h>

namespace idk
{
	struct ray
	{
		vec3 origin;
		vec3 direction;

		// returns a point at distance units along the ray
		vec3 get_point(real distance) const;
	};
}