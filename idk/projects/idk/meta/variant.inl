#pragma once
#include "variant.h"
namespace idk
{
	namespace detail
	{
		template<typename T>
		struct variant_helper;

		template<typename ... Ts>
		struct variant_helper<std::variant<Ts...>>
		{
			static constexpr auto ConstructJT()
			{
				using T = std::variant<Ts...>;
				return std::array<T(*)(), sizeof...(Ts)>
				{
					[]() -> T
					{
						return Ts{};
					} ...
				};
			}
		};
	}
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


	template<typename T>
	constexpr auto variant_construct(size_t i) noexcept
	{
		constexpr auto jt = detail::variant_helper<T>::ConstructJT();
		return jt[i]();
	}



    template<typename FindMe>
    struct index_in_variant<FindMe, std::variant<>>
    {
        static constexpr uint8_t value = 0;
    };

    template<typename FindMe, typename ... Ts>
    struct index_in_variant<FindMe, std::variant<FindMe, Ts...>>
    {
        static constexpr uint8_t value = 0;
    };

    template<typename FindMe, typename First, typename ... Ts>
    struct index_in_variant<FindMe, std::variant<First, Ts...>>
    {
        static constexpr uint8_t value = index_in_variant<FindMe, std::variant<Ts...>>::value + 1;
    };
}