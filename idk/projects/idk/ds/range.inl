#pragma once
#include <utility>

namespace idk
{
	namespace detail
	{
		template<typename T, size_t ... Ns>
		constexpr auto index_to_array(std::index_sequence<Ns...>) noexcept
		{
			return std::array<T, sizeof...(Ns)>{static_cast<T>(Ns)...};
		}
	}
	// compile time
	template<size_t stop, typename T>
	constexpr auto range() noexcept
	{
		return detail::index_to_array<T>(std::make_index_sequence<stop>{});
	}
}