#pragma once
#include <idk.h>

namespace idk
{
	struct halfspace
	{
		vec3 normal{}; // always normalized
		real dist{};

		explicit halfspace(const vec4& plane_eqn); // plane equation constructor
		halfspace(const vec3& normal = vec3{ 1,0,0 }, const vec3& pt = vec3{});

		bool contains(const vec3& point) const;
		vec3 origin_pt() const;

		halfspace& operator*=(const mat4& mat);
		halfspace  operator* (const mat4& mat) const;
	};
}