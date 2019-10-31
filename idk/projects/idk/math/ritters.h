#pragma once
#include <math/vector.h>
#include <math/shapes/sphere.h>

namespace idk
{
	sphere ritters(span<vec3> positions);
	sphere ritters(span<const vec3> positions);
}