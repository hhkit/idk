#pragma once
#include <type_traits>
#include "meta.h"

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



    // true if is integral/floating point, or is constructible from and to string, or is macro enum
    template<typename T>
    struct is_basic_serializable : std::disjunction<
        std::is_arithmetic<std::decay_t<T>>,
        is_macro_enum<std::decay_t<T>>,
        std::conjunction<std::is_constructible<string, T>, std::is_constructible<std::decay_t<T>, string>>> {};
}