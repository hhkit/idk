#pragma once
#include <cmath>
#include "arith.h"
#include "angle.h"

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
	template<typename T, typename LerpFactor>
	T slerp(T lhs, T rhs, LerpFactor lerp, const math::radians<T>& ohm, T sin_ohm)
	{
		constexpr auto _1 = T{ 1 };
		return sin(ohm * (_1 - lerp)) / sin_ohm * lhs + sin(ohm * lerp) / sin_ohm * rhs;
	}
	template<typename T, typename LerpFactor>
	T slerp(T lhs, T rhs, LerpFactor lerp)
	{
		auto ohm = acos(lhs.dot(rhs));
		return slerp(lhs, rhs, ohm, sin(ohm));
	}
}