#pragma once
#include <idk.h>

namespace idk
{
	struct sphere
	{
		vec3 center;
		real radius { 1.f };

		// accessors
		real diameter() const;

		// properties
		sphere& translate(const vec3& trans);
		sphere& center_at(const vec3& pos);
		sphere& surround(vec3 point);
		sphere& grow(real distance);
		sphere& operator*=(const mat4& tfm);
		sphere  operator*(const mat4& tfm) const;

		// collision
		bool contains(vec3 point) const;
		bool contains(const sphere& s) const;
		bool overlaps(const sphere& s) const;
	};
}