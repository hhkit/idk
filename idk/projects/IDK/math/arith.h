#pragma once

namespace idk
{
	template<typename T>
	T fmod(T numerator, T denominator);

	template<typename T, typename LerpFactor>
	T lerp(T lhs, T rhs, LerpFactor lerp);

	template<typename T, typename LerpFactor>
	T slerp(T lhs, T rhs, LerpFactor lerp);
}

#include "arith.inl"