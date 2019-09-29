#pragma once

#include <idk.h>

namespace idk
{
	struct euler_angles
	{
		rad x, y, z;

		euler_angles() = default;
		explicit euler_angles(quat q);
		explicit operator quat();
	};
}