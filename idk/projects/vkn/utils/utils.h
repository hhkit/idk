#pragma once
#include <iosfwd>
#include <idk.h>

namespace idk::vkn::hlp {

	std::ostream& cerr();

	template<typename K, typename V>
	hash_table<V, K> ReverseMap(const hash_table<K, V>& map);
}
namespace idk::vkn::meta
{
	template<typename V, typename T>
	struct IndexOf;

	template<typename T, T v, typename Pack>
	struct IndexInPack;
	template<typename Pack>
	struct PackSize;
	template<size_t index, typename Pack>
	struct Get;

	template<typename Enum, Enum ...Args>
	struct enum_pack;

	template<typename Tuple>
	struct to_array;
	template<typename T, template<typename,T...> typename Pack, T...Args>
	struct to_array<Pack<T,Args...>>
	{
		static constexpr const T(&value())[sizeof...(Args)]
		{
			static constexpr T arr[] = {Args...};
			return arr;
		}
	};

	template<typename Pack>
	struct IsSequential;

	template<typename Enum, typename Tuple = void>
	struct enum_info
	{
		static Enum map(size_t index);
		static size_t map(Enum index);
		static std::optional<size_t> map_maybe(Enum index);

		static constexpr size_t size();

	template<Enum e>
		static constexpr uint32_t map();

	template<uint32_t n>
		static constexpr Enum map();
	};

	template<typename >
	struct always_true :std::true_type {};
	template<typename ...>
	struct always_true_va :std::true_type {};

	template<size_t N, template<typename T> typename cond = always_true>
	struct TupleFuncForward
	{
		template<typename Tuple, typename Fn, typename ...Args>
		inline static void invoke(Tuple& tuple, Fn&& func, Args&& ...args);
	};

	template<template<typename... >typename SFINAE, typename ...Tz>
	struct SfinaeBool;
}
namespace std
{
	template<typename T>
	struct sizer;

	template<typename T, size_t N>
	struct sizer<T(&)[N]>
	{
		static constexpr size_t value = N;
	};
	template<typename T>
	constexpr static size_t size_v = sizer<T>::value;
}
