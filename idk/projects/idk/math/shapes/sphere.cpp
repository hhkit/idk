#include "stdafx.h"
#include "sphere.h"
#include <cmath>

namespace idk
{
	real sphere::diameter() const
	{
		return radius * 2;
	}

	aabb sphere::bounds() const
	{
		return aabb{ center + vec3{radius}, center - vec3{radius} };
	}

	sphere& sphere::translate(const vec3& trans)
	{
		center += trans;
		return *this;
	}

	sphere& sphere::center_at(const vec3& pos)
	{
		center = pos;
		return *this;
	}

	sphere& sphere::surround(vec3 point)
	{
		radius = std::max(center.distance(point), radius);
		return *this;
	}

	sphere& sphere::grow(real distance)
	{
		radius += distance;
		return *this;
	}

	sphere& sphere::operator*=(const mat4& tfm)
	{
		center = tfm * vec4{ center, 1 };
		auto max_len = real{};
		std::for_each(tfm.begin(), tfm.end() - 1, [&max_len](auto col) { max_len = std::max(col.length_sq(), max_len); });
		radius *= sqrt(max_len);
		return *this;
	}

	sphere sphere::operator*(const mat4& tfm) const
	{
		auto copy = *this;
		return copy *= tfm;
	}

	bool sphere::contains(vec3 point) const
	{
		return center.distance_sq(point) - radius * radius < +epsilon;
	}

	bool sphere::contains(const sphere& s) const
	{
		return center.distance_sq(s.center) - (radius - s.radius) < +epsilon;
	}

	bool sphere::overlaps(const sphere& s) const
	{
		return center.distance_sq(s.center) - (radius + s.radius) * (radius + s.radius) < +epsilon;
	}
}