#pragma once
#include <cmath>
namespace idk
{
	template<typename T>
	T fmod(T numerator, T denominator)
	{
		auto reps = floor(numerator / denominator);
		return numerator - reps * denominator;
	}
}