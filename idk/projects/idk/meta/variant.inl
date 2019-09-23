#pragma once
namespace idk
{
	template<template<class T> typename Template, typename ...Ts>
	struct variant_wrap <std::variant<Ts...>, Template>
	{
		using type = std::variant<Template<Ts>...>;
	};

	template<typename ... Ts>
	struct tuple_to_variant <std::tuple<Ts...>>
	{
		using type = std::variant<Ts...>;
	};
}