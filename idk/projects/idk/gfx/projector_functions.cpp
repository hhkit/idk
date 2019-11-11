#include "stdafx.h"
#include "projector_functions.h"

namespace idk
{
	vec2 spherical_projection(vec3 vec)
	{
		auto r = vec.length();
		const auto v = acos(vec.z / r);

		return vec2{ atan2(vec.y, vec.x) / two_pi + 0.5f, v / rad{ pi } };
	}

	std::pair<vec2, TextureTarget> cube_projection(vec3 vec)
	{
		const float absX = fabs(vec.x);
		const float absY = fabs(vec.y);
		const float absZ = fabs(vec.z);

		const int isXPositive = vec.x > 0 ? 1 : 0;
		const int isYPositive = vec.y > 0 ? 1 : 0;
		const int isZPositive = vec.z > 0 ? 1 : 0;

		float maxAxis = 0.f, uc = 0.f, vc = 0.f;

		TextureTarget index;

		// POSITIVE X
		if (isXPositive && absX >= absY && absX >= absZ) {
			// u (0 to 1) goes from +z to -z
			// v (0 to 1) goes from -y to +y
			maxAxis = absX;
			uc = -vec.z;
			vc = vec.y;
			index = TextureTarget::PosX;
		}
		else // NEGATIVE X
		if (!isXPositive && absX >= absY && absX >= absZ) {
			// u (0 to 1) goes from -z to +z
			// v (0 to 1) goes from -y to +y
			maxAxis = absX;
			uc = vec.z;
			vc = vec.y;
			index = TextureTarget::NegX;
		}
		else // POSITIVE Y
		if (isYPositive && absY >= absX && absY >= absZ) {
			// u (0 to 1) goes from -x to +x
			// v (0 to 1) goes from +z to -z
			maxAxis = absY;
			uc = vec.x;
			vc = -vec.z;
			index = TextureTarget::PosY;
		}
		else // NEGATIVE Y
		if (!isYPositive && absY >= absX && absY >= absZ) {
			// u (0 to 1) goes from -x to +x
			// v (0 to 1) goes from -z to +z
			maxAxis = absY;
			uc = vec.x;
			vc = vec.z;
			index = TextureTarget::NegY;
		}
		// POSITIVE Z
		else if (isZPositive && absZ >= absX && absZ >= absY) {
			// u (0 to 1) goes from -x to +x
			// v (0 to 1) goes from -y to +y
			maxAxis = absZ;
			uc = vec.x;
			vc = vec.y;
			index = TextureTarget::PosZ;
		}
		// NEGATIVE Z
		else if (!isZPositive && absZ >= absX && absZ >= absY) {
			// u (0 to 1) goes from +x to -x
			// v (0 to 1) goes from -y to +y
			maxAxis = absZ;
			uc = -vec.x;
			vc = vec.y;
			index = TextureTarget::NegZ;
		}

		// Convert range from -1 to 1 to 0 to 1
		return std::make_pair(vec2{ 0.5f * (uc / maxAxis + 1.0f), 0.5f * (vc / maxAxis + 1.0f) }, index);
	}

	
}
