#pragma once

namespace idk
{
	template<typename T, typename U>
	constexpr T s_cast(U&& rhs) noexcept;

	template<typename T, typename U>
	constexpr T d_cast(U&& rhs) noexcept;

	template<typename T, typename U>
	constexpr T r_cast(U&& rhs) noexcept;

	template<typename T, typename U>
	constexpr T c_cast(U&& rhs) noexcept;

	template<typename T>
	constexpr auto&& fwd(T&& fwdee) noexcept;
}

#include "casts.inl"