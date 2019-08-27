#pragma once

#include <idk.h>

namespace idk
{
	struct euler_angles
	{
		rad x, y, z;

		explicit euler_angles(quat q);
		explicit operator quat();
	};
}