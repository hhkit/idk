#pragma once
namespace idk
{
	namespace math {
		template<typename T> struct trad;
	}

	template<typename T>
	T frem(T numerator, T denominator);

	template<typename T>
	T fmod(T numerator, T denominator);

	template<typename T, typename LerpFactor>
	T lerp(T lhs, T rhs, LerpFactor lerp);

	template<typename Vec, typename LerpFactor>
	[[nodiscard]] Vec slerp(const Vec& lhs, const Vec& rhs, LerpFactor lerp, const trad<LerpFactor>& angle_between, LerpFactor sin_angle_between);

	template<typename Vec, typename LerpFactor>
	[[nodiscard]] Vec slerp(const Vec& lhs, const Vec& rhs, LerpFactor lerp);
}

#include "arith.inl"