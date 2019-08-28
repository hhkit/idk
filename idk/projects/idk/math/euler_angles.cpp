#include "stdafx.h"
#include "euler_angles.h"

namespace idk
{

	euler_angles::euler_angles(quat q)
	{
		x = rad(atan2(q.w * q.x + q.y * q.z, 0.5f - (q.x * q.x + q.y * q.y)));

		real t2 = 2.0f * (q.w * q.y - q.z * q.x);
		if (fabsf(t2) >= 1.0f) // gimbal
			y = rad(copysignf(half_pi, t2));
		else
			y = rad(asin(t2));
		z = rad(atan2(q.w * q.z + q.x * q.y, 0.5f - (q.y * q.y + q.z * q.z)));
	}

	euler_angles::operator quat()
	{
		real cx = cosf(x.value * 0.5f);
		real sx = sinf(x.value * 0.5f);
		real cy = cosf(y.value * 0.5f);
		real sy = sinf(y.value * 0.5f);
		real cz = cosf(z.value * 0.5f);
		real sz = sinf(z.value * 0.5f);

		return quat
		{
			cz * cy * sx - sz * sy * cx,
			sz * cy * sx + cz * sy * cx,
			sz * cy * cx - cz * sy * sx,
			cz * cy * cx + sz * sy * sx
		};
	}

}