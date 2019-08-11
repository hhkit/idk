#pragma once
namespace idk
{
	namespace math {
		template<typename T> struct radians;
	}

	template<typename T>
	T fmod(T numerator, T denominator);

	template<typename T, typename LerpFactor>
	T lerp(T lhs, T rhs, LerpFactor lerp);

	template<typename Vec, typename LerpFactor>
	Vec slerp(const Vec& lhs, const Vec& rhs, LerpFactor lerp, const math::radians<LerpFactor>& angle_between, LerpFactor sin_angle_between);

	template<typename Vec, typename LerpFactor>
	Vec slerp(const Vec& lhs, const Vec& rhs, LerpFactor lerp);
}

#include "arith.inl"