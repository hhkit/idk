#pragma once

#include <idk.h>

namespace idk
{
	struct euler_angles
	{
		rad x, y, z;

        constexpr euler_angles(rad x, rad y, rad z) : x{ x }, y{ y }, z{ z } {}
		explicit euler_angles(quat q);

		constexpr euler_angles() = default;
		constexpr euler_angles(const euler_angles&) = default;
		constexpr euler_angles(euler_angles&&) = default;

        euler_angles& operator=(const euler_angles&) = default;
        euler_angles& operator=(euler_angles&&) = default;

		explicit operator quat() const;
		explicit operator mat3() const;
	};
}