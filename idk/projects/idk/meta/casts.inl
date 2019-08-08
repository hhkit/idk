#pragma once
#include <utility>
#include "casts.h"

namespace idk
{
	template<typename T, typename U>
	constexpr T s_cast(U&& rhs) noexcept
	{
		return static_cast<T>(std::forward<U>(rhs));
	}

	template<typename T, typename U>
	constexpr T d_cast(U&& rhs) noexcept
	{
		return dynamic_cast<T>(std::forward<U>(rhs));
	}

	template<typename T, typename U>
	constexpr T r_cast(U&& rhs) noexcept
	{
		return reinterpret_cast<T>(std::forward<U>(rhs));
	}

	template<typename T, typename U>
	constexpr T c_cast(U&& rhs) noexcept
	{
		return const_cast<T>(std::forward<U>(rhs));
	}
	template<typename T>
	constexpr auto&& fwd(T&& fwdee) noexcept
	{
		return static_cast<T&&>(fwdee);
	}
}