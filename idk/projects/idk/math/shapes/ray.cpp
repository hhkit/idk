#include "stdafx.h"
#include "ray.h"

namespace idk
{
	vec3 ray::direction() const
	{
		return velocity.get_normalized();
	}
	vec3 ray::get_point(real distance) const
	{
		return origin + distance * velocity.get_normalized();
	}

	vec3 ray::get_point_after(real t) const
	{
		return origin + velocity * t;
	}

}