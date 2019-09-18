#pragma once
#include <idk.h>
#include <math/shapes/halfspace.h>
#include <util/enum.h>

namespace idk
{
	ENUM(FrustumSide, char,
		Right,
		Left,
		Up,
		Down,
		Far,
		Near
	)

	struct frustum
	{
		array<halfspace, FrustumSide::count> sides;

		frustum& operator*=(const mat4& tfm);
		frustum  operator*(const mat4& tfm) const;
	};

	frustum camera_vp_to_frustum(const mat4& view_projection_matrix);
}