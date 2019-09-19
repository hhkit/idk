#include "pch.h"
#include <math/shapes/sphere.h>
#undef min
#undef max
TEST(TestGeometry, TestSphere)
{
	using namespace idk;
	auto sph = sphere{ vec3{}, 5 };

	auto bounds = sph.bounds();
	assert(bounds.min.x < bounds.max.x);
	assert(bounds.min.y < bounds.max.y);
	assert(bounds.min.z < bounds.max.z);
}