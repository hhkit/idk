#include "stdafx.h"
#include "AnimationUtils.h"
#include <math/constants.h>
namespace idk::anim
{
	float piecewise_linear(float prev, float curr, float next, float val)
	{
		if (val < prev || val > next)
			return 0.0f;

		float divisor = val <= curr ? curr - prev : curr - next;

		// If curr - next or curr - prev is the same, then we return 
		if (abs(divisor) < 0.0001f)
		{
			// Special case when val == curr and curr == prev
			if (abs(val - curr) < 0.0001f)
				return 1.0f;
			return 0.0f;
		}

		float res = val <= curr ? val - prev : val - next;
		return res / divisor;
	}
}