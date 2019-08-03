#pragma once
#include <cmath>
#include "arith.h"
namespace idk
{
	template<typename T>
	T fmod(T numerator, T denominator)
	{
		auto reps = floor(numerator / denominator);
		return numerator - reps * denominator;
	}
	template<typename T, typename LerpFactor>
	T lerp(T lhs, T rhs, LerpFactor lerp)
	{
		return lhs * (LerpFactor(1) - lerp) + rhs * lerp;
	}
}