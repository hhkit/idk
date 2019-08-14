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
	template<typename Vec, typename LerpFactor>
	Vec slerp(const Vec& lhs, const Vec& rhs, LerpFactor lerp, const math::radians<LerpFactor>& ohm, LerpFactor sin_ohm)
	{
		constexpr auto _1 = LerpFactor{ 1 };
		return sin(ohm * (_1 - lerp)) / sin_ohm * lhs + sin(ohm * lerp) / sin_ohm * rhs;
	}
	template<typename Vec, typename LerpFactor>
	Vec slerp(const Vec& lhs, const Vec& rhs, LerpFactor lerp)
	{
		auto ohm = acos(lhs.get_normalized().dot(rhs.get_normalized()));
		return slerp(lhs, rhs, ohm, sin(ohm));
	}
}