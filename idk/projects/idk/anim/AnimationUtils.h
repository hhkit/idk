#pragma once
#include <idk.h>
#include <util/enum.h>
namespace idk
{
	ENUM(AnimDataType, char,
		NONE,
		BOOL, 
		INT,
		FLOAT
	);

	ENUM(BlendTreeType, char,
		BlendTree_1D,
		BlendTree_2D_Catesian,
		BlendTree_2D_Directional
	);

	static float piecewise_linear(float prev, float curr, float next, float val);

	// vector<float> gradient_band_interp();

}
#include "AnimationUtils.inl"