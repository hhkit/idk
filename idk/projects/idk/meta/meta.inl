#pragma once
#include <type_traits>

namespace idk
{
	template<typename T, template<typename ...> typename TMPLT>
	struct is_template
		: std::false_type
	{
	};

	template<template<typename ...> typename TMPLT, typename...Args>
	struct is_template<TMPLT<Args...>, TMPLT>
		: std::true_type
	{
	};

	template<typename ... Ts, template<typename ...> typename Wrap>
	struct tuple_wrap<std::tuple<Ts...>, Wrap>
	{
		using type = std::tuple<Wrap<Ts>...>;
	};

	template<typename FindMe>
	struct index_in_tuple < FindMe, std::tuple<>>
	{
		static constexpr uint8_t value = 0;
	};

	template<typename FindMe, typename ... Ts>
	struct index_in_tuple < FindMe, std::tuple<FindMe, Ts...>>
	{
		static constexpr uint8_t value = 0;
	};

	template<typename FindMe, typename First, typename ... Ts>
	struct index_in_tuple < FindMe, std::tuple<First, Ts...>>
	{
		static constexpr uint8_t value = index_in_tuple<FindMe, std::tuple<Ts...>>::value + 1;
	};

	template<typename T, T ... Indexes1, T ... Indexes2>
	struct index_sequence_cat<std::index_sequence<Indexes1...>, std::index_sequence<Indexes2...>>
	{
		using type = std::index_sequence<Indexes1..., Indexes2...>;
	};

	template<>
	struct index_sequence_rev<std::index_sequence<>>
	{
		using type = std::index_sequence<>;
	};

	template<typename T, T Last, T ... Indexes>
	struct index_sequence_rev<std::index_sequence<Indexes..., Last>>
	{
		using type = index_sequence_cat_t<std::index_sequence<Last>, index_sequence_rev_t<std::index_sequence<Indexes...>>>;
	};
}