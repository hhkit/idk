#pragma once
#include "AnimationUtils.h"
#include <math/constants.h>
namespace idk
{
	static float piecewise_linear(float prev, float curr, float next, float val)
	{
		float divisor = val < curr ? curr - next : curr - prev;
		if (abs(divisor) == constants::epsilon<float>())
			return float{1};

		float res = val < curr ? val - next : val - prev;
		return res / divisor;
	}
}