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

	template<typename T, typename LerpFactor>
	T slerp(T lhs, T rhs, LerpFactor lerp, const math::radians<T>& angle_between, T sin_angle_between);

	template<typename T, typename LerpFactor>
	T slerp(T lhs, T rhs, LerpFactor lerp);
}

#include "arith.inl"