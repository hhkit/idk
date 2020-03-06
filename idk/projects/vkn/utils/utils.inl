#pragma once
#include "utils.h"
namespace idk::vkn::hlp
{

	template<typename K, typename V>
	hash_table<V, K> ReverseMap(const hash_table<K, V>& map)
	{
		hash_table<V, K> result;
		for (auto& [k, v] : map)
		{
			result.emplace(v, k);
		}
		return result;}
}
namespace idk::vkn
{

}
namespace idk::vkn::meta
{
	namespace detail
	{
		template<typename T, typename ...Args>
		struct MatchIndex {
			static uint32_t constexpr value() { return 0; }
		};
		template<typename T, typename Front, typename ...Args>
		struct MatchIndex<T, Front, Args...>
		{
			static uint32_t constexpr value()
			{
				if constexpr (std::is_same_v<Front, T>)
				{
					return 0;
				}
				else
				{
					return MatchIndex<T, Args...>::value() + 1;
				}
			};
		};


		template<typename T, T val, T ...Args>
		struct MatchIndexV {
			static uint32_t constexpr value() { return 0; }
		};
		template<typename T, T val, T Front, T ...Args>
		struct MatchIndexV<T,val, Front, Args...>
		{
			static uint32_t constexpr value()
			{
				if constexpr (val==Front)
				{
					return 0;
				}
				else
				{
					return MatchIndexV<T,val, Args...>::value() + 1;
				}
			};
		};
		template<size_t N, typename T, T front, T ...Args>
		struct Count
		{
			static constexpr T value = Count<N-1,T,Args...>::value;
		};
		template<typename T, T front, T ...Args>
		struct Count<0,T,front,Args...>
		{
			static constexpr T value= front;
		};
		template<size_t N, typename T, T ...Args>
		struct Count<N,T,Args...>
		{
			//static_assert(false, "Count out of bounds!");
		};



		template<bool>
		struct Invoker {
			template<typename ...Args> static void Invoke(Args&& ...) {}
		};
		template<>
		struct Invoker <true>
		{
			template<typename Elem, typename Fn, typename ...Args>
			static void Invoke(Elem& tup, Fn&& f, Args&& ... args)
			{
				f(tup, std::forward<Args>(args)...);
			}
		};

		template<size_t N, template<typename > typename cond = always_true>
		struct TupleFuncForward
		{
			template<typename Tuple, typename Fn, typename ...Args>
			inline static void invoke(Tuple& tuple, Fn&& func, Args&& ...args)
			{
				TupleFuncForward<N - 1, cond>::invoke(tuple, std::forward<Fn>(func), std::forward<Args>(args)...);
				Invoker< cond<std::tuple_element_t<N, Tuple>>::value>::Invoke(std::get<N>(tuple), func, std::forward<Args>(args)...);
			}
		};


		template<template<typename T> typename cond>
		struct TupleFuncForward<0, cond>
		{
			template<typename Tuple, typename Fn, typename ...Args>
			inline static void invoke(Tuple& tuple, Fn&& func, Args&& ...args)
			{
				//if constexpr (cond<std::tuple_element_t<0, Tuple>>::value)
				//	func(std::get<0>(tuple), std::forward<Args>(args)...);
				Invoker< cond<std::tuple_element_t<0, Tuple>>::value>::Invoke(std::get<0>(tuple), func, std::forward<Args>(args)...);
			}
		};
		template<template<typename >typename Cond = always_true, typename Tuple, typename Fn, typename ...Args>
		void for_each_tuple_element(Tuple&& tuple, Fn&& func, Args&& ... args)
		{
			constexpr size_t N = std::tuple_size_v<std::remove_reference_t<Tuple>>;
			if constexpr (N!=0)
				TupleFuncForward<N - 1, Cond>::invoke(std::forward<Tuple>(tuple), std::forward<Fn>(func), std::forward<Args>(args)...);
		}
		template<template<typename... >typename SFINAE, typename ...Tz>
		struct SfinaeBool
		{
			template<typename ...T, typename = SFINAE<T...>>
			static bool func(T && ...);

			static float func(...);


			static constexpr bool value = std::is_same_v< decltype(func(std::declval<Tz>()...)), bool>;
		};

		template<typename Pack>
		struct IsSequential : std::false_type
		{
		};

		template<template<typename T, T ...> typename Pack, typename T, T...Args>
		struct IsSequential<Pack<T, Args...>>
		{
			static constexpr bool value = std::is_same_v<std::integer_sequence<size_t, static_cast<size_t>(Args)...>, std::make_integer_sequence<size_t, sizeof...(Args)>>;
		};

		template<typename Pack1, typename Pack2>
		struct MakeMap;


		template<template<typename T1, T1...>typename Pack1, template<typename T2, T2...>typename Pack2, typename T1, typename T2, T1...Args1, T2...Args2>
		struct MakeMap<Pack1<T1, Args1...>, Pack2<T2, Args2...>>
		{
			static const auto make()
			{
				return hash_table<T1, T2>{ {Args1, Args2}...};
			}
		};


		template<typename Enum, typename Tuple, bool is_convertible>
		struct MappedEnum
		{
			inline static const size_t map(Enum e) { return static_cast<size_t>(e); }
			inline static const std::optional<size_t> map_maybe(Enum e) { return static_cast<size_t>(e); }
		};

		template<typename Enum, template<typename, Enum...>typename  Pack, Enum...Args>
		struct MappedEnum<Enum,Pack<Enum,Args...>,false>
		{
			inline static size_t map(Enum e)
			{
				static const auto map = MakeMap<Pack<Enum, Args...>, std::make_index_sequence<sizeof...(Args)>>::make();
				return map.find(e)->second;
			}
			inline static auto map_maybe(Enum e)
			{
				static const auto map = MakeMap<Pack<Enum, Args...>, std::make_index_sequence<sizeof...(Args)>>::make();
				std::optional<size_t> result{};
				auto itr = map.find(e);
				if (itr != map.end())
					result=itr->second;
				return result;
			}
		};



	}

	template<typename Pack>
	struct IsSequential : detail::IsSequential<Pack> {};
	template<size_t N, template<typename T> typename cond>
	template<typename Tuple, typename Fn, typename ...Args>
	void TupleFuncForward<N, cond>::invoke(Tuple& tuple, Fn&& func, Args&& ...args)
	{
		detail::TupleFuncForward<N, cond>::invoke(tuple, std::forward<Fn>(func), std::forward<Args>(args)...);
	}

	template<template<typename >typename Cond = always_true, typename Tuple, typename Fn, typename ...Args>
	inline void for_each_tuple_element(Tuple&& tuple, Fn&& func, Args&& ... args)
	{
		detail::for_each_tuple_element<Cond>(std::forward<Tuple>(tuple), std::forward<Fn>(func), std::forward<Args>(args)...);
	}
	template<typename Enum, typename Tuple>
	Enum enum_info<Enum, Tuple>::map(size_t index)
	{
		static auto& test = to_array<Tuple>::value();
		return test[index];
	}

	template<typename Enum, typename Tuple>
	size_t enum_info<Enum, Tuple>::map(Enum e)
	{
		return detail::MappedEnum<Enum, Tuple, IsSequential<Tuple>::value>::map(e);
	}
	template<typename Enum, typename Tuple>
	std::optional<size_t> enum_info<Enum, Tuple>::map_maybe(Enum e)
	{
		return detail::MappedEnum<Enum, Tuple, IsSequential<Tuple>::value>::map_maybe(e);
	}
	template<typename Enum, typename Tuple>
	constexpr size_t enum_info<Enum, Tuple>::size()
	{
		return PackSize<Tuple>::value;
	}
	template<typename Enum, typename Tuple>
	template<Enum e>
	constexpr uint32_t enum_info<Enum, Tuple>::map()
	{
		return IndexInPack<Enum, e, Tuple>::value;
	}
	template<typename Enum, typename Tuple>
	template<uint32_t n>
	constexpr Enum enum_info<Enum, Tuple>::map()
	{
		return Get<static_cast<size_t>(n), Tuple>::value();
	}

		template<typename ...Args,template <typename ...> typename V, typename T>
		struct IndexOf< V<Args...>, T>
		{
			static constexpr uint32_t value = detail::MatchIndex<T, Args...>::value();
		};

		template<typename T, T v, template <typename, T...> typename Pack, T ... Args>
		struct IndexInPack<T, v, Pack<T, Args...>>
		{
			static constexpr uint32_t value = detail::MatchIndexV<T, v, Args...>::value();
		};


		template<typename T, template<typename, T...>typename Pack, T...Args>
		struct PackSize<Pack<T, Args...>>
		{
			static constexpr size_t value = sizeof...(Args);
		};

		template<size_t index, typename T, template<typename, T...>typename Pack, T...Args>
		struct Get<index, Pack<T, Args...>>
		{
			static constexpr T value()
			{
				return Count< index, Args...>::value;
			}
		};

		template<template<typename... >typename SFINAE, typename ...Tz>
		struct SfinaeBool
		{
			static constexpr bool value = detail::SfinaeBool<SFINAE, Tz...>::value;
		};


		template<typename V, typename T>
		static inline constexpr auto IndexOf_v = IndexOf<V, T>::value;




		namespace detail
		{
			template<typename IndexSeq>
			struct reverse_seq;
			template< size_t ... Nums>
			struct reverse_seq<std::index_sequence<Nums...>>
			{
				static constexpr size_t N = sizeof...(Nums);
				using type = std::index_sequence<(N - Nums)...>;
			};

			template<typename index_seq, typename tuple>
			struct shuffle;
			template<size_t ...indices, typename tuple>
			struct shuffle<std::index_sequence<indices...>, tuple>
			{
				using type = std::tuple<std::tuple_element_t<indices, tuple>...>;
			};

			template<typename T>
			struct Reverse;
			template<typename ...Args, template<typename...> typename tuple>
			struct Reverse<tuple<Args...>>
			{
				using seq_t = std::make_index_sequence<sizeof...(Args)>;
				using rev_seq_t = detail::reverse_seq<seq_t>;
				using type = typename detail::shuffle<rev_seq_t, tuple<Args...>>::type;
			};


			template<size_t max_items, typename T>
			struct Cull;

			template<size_t N, typename ...Args, template<typename...> typename tuple>
			struct Cull<N, tuple<Args...>>
			{
				using seq_t = std::make_index_sequence<std::min(N, sizeof...(Args))>;
				using type = typename detail::shuffle<seq_t, tuple<Args...>>::type;
			};

			template<typename func, typename tuple, typename = void>
			struct ApplierHelper2 : std::false_type {};
			template<typename Func, template<typename...>typename tuple, typename ...Args>
			struct ApplierHelper2<Func, tuple<Args...>, decltype(std::declval<Func>()(std::declval<Args&&>()...))> :std::true_type
			{
				template<typename Func, typename ...OtherArgs>
				static void apply(Func&& func, Args&&... args, OtherArgs&&...)
				{
					func(std::forward<Args>(args)...);
				}
			};
			template<size_t N, typename ...Args>
			struct ApplierHelper
			{
				template<typename Func>
				static void apply(Func&& func, Args&&... args)
				{
					using culled = typename Cull<N, std::tuple<Args...>>::type;
					using Check = ApplierHelper2<Func, culled>;
					if constexpr (Check::value)
					{
						Check::apply(std::forward<Func>(func), std::forward<Args>(args)...);
					}
					else if constexpr (N > 1)
					{
						ApplierHelper<N - 1, Args...>::apply(std::forward<Func>(func), std::forward<Args>(args)...);
					}

				}
				template<typename Func>
				void operator()(Func&& func, Args&&... args)
				{
					apply(std::forward<Func>(func), std::forward<Args>(args)...);
				}
			};
		}

		template<size_t N, typename tuple>
		using cull_t = detail::Cull<N, tuple>;


		template<typename ...Args>
		struct Applier : detail::ApplierHelper<sizeof...(Args), Args...>
		{
			using base = detail::ApplierHelper<sizeof...(Args), Args...>;
			using base::operator(); //use all the other valid functors
		};

}