#include "stdafx.h"
#include "ray.h"

namespace idk
{
	vec3 ray::get_point(real distance) const
	{
		return origin + distance * direction.get_normalized();
	}

}