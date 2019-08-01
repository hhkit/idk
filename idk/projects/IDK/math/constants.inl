#pragma once

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
}