#pragma once
#include <type_traits>

#include "constants.h"

namespace idk::constants
{
	template<>
	constexpr float pi<float>()
	{
		return 3.1415926535f;
	}

	template<>
	constexpr double pi<double>()
	{
		return 3.141592653589793;
	}

	template<>
	constexpr float tau<float>()
	{
		return 6.28318530717958f;
	}

	template<>
	constexpr double tau<double>()
	{
		return 6.28318530717958;
	}

	template<typename T>
	constexpr T epsilon()
	{
		if constexpr (std::is_same_v<T, float>)
			return 0.0001f;
		else if constexpr (std::is_same_v<T, double>)
			return 0.0000001;
		else // integral type
			return T{};
	}

}