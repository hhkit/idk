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
	bool frustum::contains(const sphere& s) const
	{
		for (auto& side : sides)
		{
			const float dist_to_point = side.normal.dot(s.center) + side.dist;
			if (dist_to_point > s.radius)
			{
				return false;
			}
		}
		return true;
	}
	frustum camera_vp_to_frustum(const mat4& view_projection_matrix)
	{
		frustum result{};
		std::array<vec4, FrustumSide::count> planes;
		planes[FrustumSide::Left].x = -(view_projection_matrix[0][3] + view_projection_matrix[0][0]);
		planes[FrustumSide::Left].y = -(view_projection_matrix[1][3] + view_projection_matrix[1][0]);
		planes[FrustumSide::Left].z = -(view_projection_matrix[2][3] + view_projection_matrix[2][0]);
		planes[FrustumSide::Left].w = -(view_projection_matrix[3][3] + view_projection_matrix[3][0]);

		planes[FrustumSide::Right].x = -(view_projection_matrix[0][3] - view_projection_matrix[0][0]);
		planes[FrustumSide::Right].y = -(view_projection_matrix[1][3] - view_projection_matrix[1][0]);
		planes[FrustumSide::Right].z = -(view_projection_matrix[2][3] - view_projection_matrix[2][0]);
		planes[FrustumSide::Right].w = -(view_projection_matrix[3][3] - view_projection_matrix[3][0]);

		planes[FrustumSide::Down].x = -(view_projection_matrix[0][3] + view_projection_matrix[0][1]);
		planes[FrustumSide::Down].y = -(view_projection_matrix[1][3] + view_projection_matrix[1][1]);
		planes[FrustumSide::Down].z = -(view_projection_matrix[2][3] + view_projection_matrix[2][1]);
		planes[FrustumSide::Down].w = -(view_projection_matrix[3][3] + view_projection_matrix[3][1]);

		planes[FrustumSide::Up].x = -(view_projection_matrix[0][3] - view_projection_matrix[0][1]);
		planes[FrustumSide::Up].y = -(view_projection_matrix[1][3] - view_projection_matrix[1][1]);
		planes[FrustumSide::Up].z = -(view_projection_matrix[2][3] - view_projection_matrix[2][1]);
		planes[FrustumSide::Up].w = -(view_projection_matrix[3][3] - view_projection_matrix[3][1]);

		planes[FrustumSide::Near].x = -(view_projection_matrix[0][3] + view_projection_matrix[0][2]);
		planes[FrustumSide::Near].y = -(view_projection_matrix[1][3] + view_projection_matrix[1][2]);
		planes[FrustumSide::Near].z = -(view_projection_matrix[2][3] + view_projection_matrix[2][2]);
		planes[FrustumSide::Near].w = -(view_projection_matrix[3][3] + view_projection_matrix[3][2]);

		planes[FrustumSide::Far].x = -(view_projection_matrix[0][3] - view_projection_matrix[0][2]);
		planes[FrustumSide::Far].y = -(view_projection_matrix[1][3] - view_projection_matrix[1][2]);
		planes[FrustumSide::Far].z = -(view_projection_matrix[2][3] - view_projection_matrix[2][2]);
		planes[FrustumSide::Far].w = -(view_projection_matrix[3][3] - view_projection_matrix[3][2]);

		result.sides[FrustumSide::Left] = halfspace{ planes[FrustumSide::Left] };
		result.sides[FrustumSide::Right] = halfspace{ planes[FrustumSide::Right] };
		result.sides[FrustumSide::Down] = halfspace{ planes[FrustumSide::Down] };
		result.sides[FrustumSide::Up] = halfspace{ planes[FrustumSide::Up] };
		result.sides[FrustumSide::Near] = halfspace{ planes[FrustumSide::Near] };
		result.sides[FrustumSide::Far] = halfspace{ planes[FrustumSide::Far] };

		return result;
	}
	aabb camera_vp_to_bounding_box(const mat4& view_projection_matrix)
	{

		//Recalculate Frustum planes based on current view matrix
		std::array<vec4, FrustumSide::count> planes;
		planes[FrustumSide::Left].x = -(view_projection_matrix[0][3] + view_projection_matrix[0][0]);
		planes[FrustumSide::Left].y = -(view_projection_matrix[1][3] + view_projection_matrix[1][0]);
		planes[FrustumSide::Left].z = -(view_projection_matrix[2][3] + view_projection_matrix[2][0]);
		planes[FrustumSide::Left].w = -(view_projection_matrix[3][3] + view_projection_matrix[3][0]);

		planes[FrustumSide::Right].x = -(view_projection_matrix[0][3] - view_projection_matrix[0][0]);
		planes[FrustumSide::Right].y = -(view_projection_matrix[1][3] - view_projection_matrix[1][0]);
		planes[FrustumSide::Right].z = -(view_projection_matrix[2][3] - view_projection_matrix[2][0]);
		planes[FrustumSide::Right].w = -(view_projection_matrix[3][3] - view_projection_matrix[3][0]);

		planes[FrustumSide::Down].x = -(view_projection_matrix[0][3] + view_projection_matrix[0][1]);
		planes[FrustumSide::Down].y = -(view_projection_matrix[1][3] + view_projection_matrix[1][1]);
		planes[FrustumSide::Down].z = -(view_projection_matrix[2][3] + view_projection_matrix[2][1]);
		planes[FrustumSide::Down].w = -(view_projection_matrix[3][3] + view_projection_matrix[3][1]);

		planes[FrustumSide::Up].x = -(view_projection_matrix[0][3] - view_projection_matrix[0][1]);
		planes[FrustumSide::Up].y = -(view_projection_matrix[1][3] - view_projection_matrix[1][1]);
		planes[FrustumSide::Up].z = -(view_projection_matrix[2][3] - view_projection_matrix[2][1]);
		planes[FrustumSide::Up].w = -(view_projection_matrix[3][3] - view_projection_matrix[3][1]);

		planes[FrustumSide::Near].x = -(view_projection_matrix[0][3] + view_projection_matrix[0][2]);
		planes[FrustumSide::Near].y = -(view_projection_matrix[1][3] + view_projection_matrix[1][2]);
		planes[FrustumSide::Near].z = -(view_projection_matrix[2][3] + view_projection_matrix[2][2]);
		planes[FrustumSide::Near].w = -(view_projection_matrix[3][3] + view_projection_matrix[3][2]);

		planes[FrustumSide::Far].x = -(view_projection_matrix[0][3] - view_projection_matrix[0][2]);
		planes[FrustumSide::Far].y = -(view_projection_matrix[1][3] - view_projection_matrix[1][2]);
		planes[FrustumSide::Far].z = -(view_projection_matrix[2][3] - view_projection_matrix[2][2]);
		planes[FrustumSide::Far].w = -(view_projection_matrix[3][3] - view_projection_matrix[3][2]);


		//Bound frustum with bounding box
		aabb boundingBox;
		boundingBox.min.x = boundingBox.min.y = boundingBox.min.z = std::numeric_limits<float>::max();
		boundingBox.max.x = boundingBox.max.y = boundingBox.max.z = std::numeric_limits<float>::min();


		for (auto& elem : planes)
		{
			vec3 pt = halfspace{elem}.origin_pt();
			boundingBox.min.x = std::min(boundingBox.min.x, pt.x);
			boundingBox.min.y = std::min(boundingBox.min.y, pt.y);
			boundingBox.min.z = std::min(boundingBox.min.z, pt.z);

			boundingBox.max.x = std::max(boundingBox.max.x, pt.x);
			boundingBox.max.y = std::max(boundingBox.max.y, pt.y);
			boundingBox.max.z = std::max(boundingBox.max.z, pt.z);
		}

		//Return tight-fitted bounding box
		return boundingBox;
	}
}
