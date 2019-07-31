#pragma once

namespace idk
{
	template<>
	constexpr float pi<float>()
	{
		return 3.1415926535f;
	}

	template<>
	constexpr double pi<double>()
	{
		return 3.14159265357989;
	}
}