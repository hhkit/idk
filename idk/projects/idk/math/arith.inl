#pragma once
#include <cmath>
#include <math/constants.h>
#include "arith.h"
#include "angle.h"

namespace idk
{
	template<typename T>
	T frem(T numerator, T denominator)
	{
		return std::fmod(numerator, denominator);
	}
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
	Vec slerp(const Vec& lhs, const Vec& rhs, LerpFactor lerp, const trad<LerpFactor>& ohm, LerpFactor sin_ohm)
	{
		constexpr auto _1 = LerpFactor{ 1 };
		return sin(ohm * (_1 - lerp)) / sin_ohm * lhs + sin(ohm * lerp) / sin_ohm * rhs;
	}
	template<typename Vec, typename LerpFactor>
	Vec slerp(const Vec& lhs, const Vec& rhs, LerpFactor lerp)
	{
		Vec rhs_copy{ rhs };
		auto cos_theta = lhs.get_normalized().dot(rhs_copy.get_normalized());

		// Shortest path
		if (cos_theta < decltype(cos_theta){0})
		{
			rhs_copy.w = -rhs_copy.w;
			rhs_copy.x = -rhs_copy.x;
			rhs_copy.y = -rhs_copy.y;
			rhs_copy.z = -rhs_copy.z;
		}

		// Divide by 0
		if (cos_theta > decltype(cos_theta){1} - constants::epsilon<decltype(cos_theta)>())
			return idk::lerp(lhs, rhs_copy, lerp);

		auto ohm = acos(cos_theta);
		return slerp(lhs, rhs_copy, lerp, ohm, sin(ohm));
	}
}