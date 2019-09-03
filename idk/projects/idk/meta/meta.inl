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



	template<typename T, typename = void>
	struct is_iterable : std::false_type {};

	template<typename T>
	struct is_iterable<T, std::void_t<decltype(
		std::begin(std::declval<T&>()) != std::end(std::declval<T&>()), // begin/end and operator !=
		void(), // Handle evil operator ,
		++std::declval<decltype(std::begin(std::declval<T&>()))&>(), // operator ++
		void(*std::begin(std::declval<T&>())) // operator*
	)>> : std::true_type {};

	template<typename T, typename = void>
	struct is_sequential_container : std::is_array<std::decay_t<T>> {};

	template<typename T>
	struct is_sequential_container<T, std::void_t<decltype(
		std::declval<T&>().front()
	)>> : is_iterable<T> {};

	template<typename T, typename = void>
	struct is_associative_container : std::false_type {};

	template<typename T>
	struct is_associative_container<T, std::void_t<decltype(
		std::declval<T&>().insert(std::declval<typename std::decay_t<T>::value_type>())
	)>> : is_iterable<T> {};


	template<typename T>
	struct is_container : std::disjunction<is_associative_container<T>, is_sequential_container<T>> {};


	template<typename T, typename = void>
	struct is_macro_enum : std::false_type {};

	template<typename T>
	struct is_macro_enum<T, std::void_t<decltype(
		std::decay_t<T>::count,
		std::decay_t<T>::names[0],
		std::decay_t<T>::values[0]
	)>> : std::is_enum<typename std::decay_t<T>::_enum> {};



	template<typename T, typename VariantT>
	struct is_variant_member;

	template<typename T, typename... Ts>
	struct is_variant_member<T, std::variant<Ts...>> : std::disjunction<std::is_same<T, Ts>...> {};



    // true if is integral/floating point, or is constructible to string
    template<typename T>
    struct is_basic_serializable : std::disjunction<
        std::is_arithmetic<T>, std::is_constructible<string, T>> {};
}