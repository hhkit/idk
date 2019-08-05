#pragma once
#include <utility>

namespace idk
{
	template<typename T, typename U>
	constexpr T s_cast(U&& rhs)
	{
		return static_cast<T>(std::forward<U>(rhs));
	}

	template<typename T, typename U>
	constexpr T d_cast(U&& rhs)
	{
		return dynamic_cast<T>(std::forward<U>(rhs));
	}

	template<typename T, typename U>
	constexpr T r_cast(U&& rhs)
	{
		return reinterpret_cast<T>(std::forward<U>(rhs));
	}

	template<typename T, typename U>
	constexpr T c_cast(U&& rhs)
	{
		return const_cast<T>(std::forward<U>(rhs));
	}
}