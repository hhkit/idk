#pragma once
#include <tuple>

#include <idk.h>

namespace idk::detail
{
	template<typename FindMe, typename Tuple>
	struct index_in_tuple;

	template<typename FindMe, typename Tuple>
	static constexpr auto index_in_tuple_v = index_in_tuple<FindMe, Tuple>::value;

	template<typename FindMe>
	struct index_in_tuple < FindMe, tuple<>>
	{
		static constexpr uint8_t value = 0;
	};

	template<typename FindMe, typename ... Ts>
	struct index_in_tuple < FindMe, tuple<FindMe, Ts...>>
	{
		static constexpr uint8_t value = 0;
	};

	template<typename FindMe, typename First, typename ... Ts>
	struct index_in_tuple < FindMe, tuple<First, Ts...>>
	{
		static constexpr uint8_t value = index_in_tuple<FindMe, tuple<Ts...>>::value + 1;
	};
}