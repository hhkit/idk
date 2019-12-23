#pragma once
#include <idk.h>
#include <math/shapes/halfspace.h>
#include <math/shapes/sphere.h>
#include <util/enum.h>

namespace idk
{
	ENUM(FrustumSide, char,
		Left,
		Right,
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

		bool contains(const sphere& s) const;
	};

	struct cascadingFrustum {
		real cascadingEnd[3];
		aabb projBox[3];
	};


	array<vec4, 8> camera_vp_to_extremes(const mat4& vp_matrix);
	frustum camera_vp_to_frustum(const mat4& view_projection_matrix);
	aabb	camera_vp_to_bounding_box(const mat4& view_projection_matrix);
	cascadingFrustum camera_vp_to_tight_bound(const mat4& view_projection_matrix);
}