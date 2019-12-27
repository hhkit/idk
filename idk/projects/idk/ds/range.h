#pragma once
#include <array>

namespace idk
{
	// dynamic range
	template<typename T> 
	constexpr auto range(T stop) noexcept;
	
	template<typename T> 
	constexpr auto range(T start, T stop, T step = 1) noexcept;

	// compile time
	template<size_t stop, typename T = size_t> 
	constexpr auto range() noexcept;
	
	template<size_t start, size_t stop, size_t step = 1> 
	constexpr auto range() noexcept;
}
