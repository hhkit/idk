#pragma once
#include <idk.h>
#include <math/shapes/aabb.h>

namespace idk
{
	struct capsule
	{
		real radius	{0.5f};
		real height	{2};
		vec3 center;
		vec3 dir	{ 0,1,0 };	// Direction of capsule faces up

		aabb bounds() const;

		bool is_sphere() const;		//When the height is smaller than the diameter of the top and bottom cap, this is true.

		capsule& operator*=(const mat4& tfm);
		capsule  operator*(const mat4& tfm) const;
	};
}