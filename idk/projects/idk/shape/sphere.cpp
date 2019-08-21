#include "stdafx.h"
#include "sphere.h"

namespace idk
{
	real sphere::diameter() const
	{
		return radius * 2;
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