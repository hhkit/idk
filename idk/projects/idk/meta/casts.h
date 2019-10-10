#pragma once
#include <machine.h>

namespace idk
{
	template<typename T, typename U>
	constexpr FORCEINLINE T s_cast(U&& rhs) noexcept;

	template<typename T, typename U>
	constexpr FORCEINLINE T d_cast(U&& rhs) noexcept;

	template<typename T, typename U>
	constexpr FORCEINLINE T r_cast(U&& rhs) noexcept;

	template<typename T, typename U>
	constexpr FORCEINLINE T c_cast(U&& rhs) noexcept;

	template<typename T>
	constexpr auto&& fwd(T&& fwdee) noexcept;
}

#include "casts.inl"