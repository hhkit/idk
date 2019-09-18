#include "stdafx.h"
#include "half_plane.h"

namespace idk
{
	halfspace::halfspace(const vec3& in_normal, const vec3& pt)
		: normal{ in_normal.get_normalized() }, dist{normal.dot(pt)}
	{
	}

	bool halfspace::contains(const vec3& point) const
	{
		return point.dot(normal) > dist;
	}

	vec3 halfspace::origin_pt() const
	{
		return normal * dist;
	}

	halfspace& halfspace::operator*=(const mat4& mat)
	{
		auto d = normal * dist; // retrieve point on plane
		normal = mat * vec4{ normal, 0 };
		dist = normal.dot(mat * vec4{ d, 1 });
		return *this;
	}

	halfspace halfspace::operator*(const mat4& mat) const
	{
		auto copy = *this;
		return copy *= mat;
	}
}
