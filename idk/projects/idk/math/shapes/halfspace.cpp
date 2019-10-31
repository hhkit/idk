#include "stdafx.h"
#include "halfspace.h"

namespace idk
{
	halfspace::halfspace(const vec4& plane_eqn)
		: normal{ plane_eqn.xyz }, dist{ plane_eqn.w }
	{
		const auto normal_len = normal.length();
		normal /= normal_len;
		dist   /= normal_len;
	}
	halfspace::halfspace(const vec3& in_normal, const vec3& pt)
		: normal{ in_normal.get_normalized() }, dist{normal.dot(pt)}
	{
	}

	bool halfspace::contains(const vec3& point) const
	{
		return point.dot(normal) + dist > 0.0f;
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

		const auto normal_len = normal.length();
		normal /= normal_len;
		dist   /= normal_len;
		return *this;
	}

	halfspace halfspace::operator*(const mat4& mat) const
	{
		auto copy = *this;
		return copy *= mat;
	}
}
