#pragma once

namespace idk::math::constants
{
	template<typename T>
	constexpr T pi();

	template<typename T>
	constexpr T tau();

	template<typename T>
	constexpr T epsilon();
}

#include "constants.inl"