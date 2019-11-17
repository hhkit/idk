#pragma once
#include <idk.h>
#include <algorithm>
#include <util/enum.h>
namespace idk::anim
{
	ENUM(AnimDataType, char,
		NONE,
		INT,
		FLOAT,
		BOOL,
		TRIGGER,
		MAX
	);

	ENUM(ConditionIndex, char,
		EQUALS,
		NOT_EQUALS,
		GREATER,
		LESS
	);

	ENUM(BlendTreeType, char,
		BlendTree_1D,
		BlendTree_2D_Catesian,
		BlendTree_2D_Directional,
		BlendTree_Max
	);

	template<typename T>
	using ConditionOp = std::function<bool(const T&, const T&)>;//bool (*)(const T&, const T&);

	inline const static ConditionOp<int> condition_ops_int[4] =
	{
		[](const int& v1, const int& v2) -> bool { return v1 == v2; },
		[](const int& v1, const int& v2) -> bool { return v1 != v2; },
		[](const int& v1, const int& v2) -> bool { return v1 > v2; },
		[](const int& v1, const int& v2) -> bool { return v1 < v2; }
	};
	inline const static ConditionOp<float> condition_ops_float[4] =
	{
		[](const float&, const float&) -> bool { return false; },
		[](const float&, const float&) -> bool { return false; },
		[](const float& v1, const float& v2) -> bool { return v1 > v2; },
		[](const float& v1, const float& v2) -> bool { return v1 < v2; }
	};

	inline const static ConditionOp<bool> condition_ops_bool[4] =
	{
		[](const bool& v1, const bool& v2) -> bool { return v1 == v2; },
		[](const bool& v1, const bool& v2) -> bool { return v1 != v2; },
		[](const bool&, const bool&) -> bool { return false; },
		[](const bool&, const bool&) -> bool { return false; }
	};
	

	float piecewise_linear(float prev, float curr, float next, float val);

	// vector<float> gradient_band_interp();

}