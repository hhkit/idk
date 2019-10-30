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
	frustum camera_vp_to_frustum(const mat4& view_projection_matrix)
	{
		frustum result{};
		std::array<vec4, FrustumSide::count> planes;
		for (int i = 4; i--; ) planes[FrustumSide::Left][i]		= view_projection_matrix[i][3] + view_projection_matrix[i][0];
		for (int i = 4; i--; ) planes[FrustumSide::Right][i]	= view_projection_matrix[i][3] - view_projection_matrix[i][0];
		for (int i = 4; i--; ) planes[FrustumSide::Down][i]		= view_projection_matrix[i][3] + view_projection_matrix[i][1];
		for (int i = 4; i--; ) planes[FrustumSide::Up][i]		= view_projection_matrix[i][3] - view_projection_matrix[i][1];
		for (int i = 4; i--; ) planes[FrustumSide::Near][i]		= view_projection_matrix[i][3] + view_projection_matrix[i][2];
		for (int i = 4; i--; ) planes[FrustumSide::Far][i]		= view_projection_matrix[i][3] - view_projection_matrix[i][2];

		result.sides[FrustumSide::Left] = halfspace{ planes[FrustumSide::Left] };
		result.sides[FrustumSide::Right] = halfspace{ planes[FrustumSide::Right] };
		result.sides[FrustumSide::Down] = halfspace{ planes[FrustumSide::Down] };
		result.sides[FrustumSide::Up] = halfspace{ planes[FrustumSide::Up] };
		result.sides[FrustumSide::Near] = halfspace{ planes[FrustumSide::Near] };
		result.sides[FrustumSide::Far] = halfspace{ planes[FrustumSide::Far] };

		return result;
	}
}
