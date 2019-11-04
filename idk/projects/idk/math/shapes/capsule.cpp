#include "stdafx.h"
#include "capsule.h"

namespace idk
{
	aabb capsule::bounds() const
	{
		auto half_extent = dir * (height) / 2 + vec3{ radius };
		
		for (auto& elem : half_extent)
			elem = abs(elem);

		return aabb{ center - half_extent, center + half_extent };
	}

	bool capsule::is_sphere() const
	{
		return height < (2*radius);
	}

	capsule& capsule::operator*=(const mat4& tfm)
	{
		center = tfm * vec4{ center, 1 };
		auto x_len_sq = tfm[0].length_sq();
		auto y_len_sq = tfm[1].length_sq();
		radius *= sqrt(std::max(y_len_sq, x_len_sq));
		height *= tfm[2].length();
		dir = tfm * vec4{ dir, 0 };
		return *this;
	}
	capsule capsule::operator*(const mat4& tfm) const
	{
		auto copy = *this;
		return copy *= tfm;
	}
}
