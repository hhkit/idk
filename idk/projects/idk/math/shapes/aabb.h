#pragma once
#include <idk.h>

namespace idk
{
	struct aabb
	{
		vec3 min;
		vec3 max;

		// accessors
		vec3  center() const;
		vec3  extents() const;
		vec3  halfextents() const;
		float volume() const;
		const aabb& bounds() const;
		aabb  combine(const aabb& rhs) const;
		bool equals(const aabb& rhs, float eps = epsilon);

		// modifiers
		aabb& translate(const vec3& trans);
		aabb& center_at(const vec3& pos);
		aabb& surround (const vec3& point);
		aabb& surround (const aabb& rhs);
		aabb& grow     (const vec3& rhs);
		

		// collision
		bool contains(const vec3& point) const;
		bool contains(const aabb& box)   const;
		bool overlaps(const aabb& box)   const;

		// transform
		aabb& operator*=(const mat4&);
		aabb  operator* (const mat4&) const;
	};
}