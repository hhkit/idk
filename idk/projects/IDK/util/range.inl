#pragma once
#include <utility>

namespace idk
{
	namespace detail
	{
		template<size_t ... Ns>
		auto index_to_array(std::index_sequence<Ns...>)
		{
			return std::array<size_t, sizeof...(Ns)>{Ns...};
		}
	}
	// compile time
	template<size_t stop>
	constexpr auto range()
	{
		return detail::index_to_array(std::make_index_sequence<stop>{});
	}
}