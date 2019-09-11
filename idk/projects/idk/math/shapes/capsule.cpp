#include "stdafx.h"
#include "capsule.h"

namespace idk
{
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
