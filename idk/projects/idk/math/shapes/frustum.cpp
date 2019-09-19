#include "stdafx.h"
#include "frustum.h"

namespace idk
{
	frustum& frustum::operator*=(const mat4& tfm)
	{
		for (auto& plane : sides)
			plane *= tfm;
		return *this;
	}
	frustum frustum::operator*(const mat4& tfm) const
	{
		auto copy = *this;
		return copy *= tfm;
	}
}
