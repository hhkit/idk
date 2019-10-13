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
	}

	template<typename ...Args, typename T>
	struct IndexOf< variant<Args...>, T>
	{
		static constexpr uint32_t value = detail::MatchIndex<T, Args...>::value();
	};

	template<typename T, T v,  template <typename,T...> typename Pack, T ... Args>
	struct IndexInPack<T,v, Pack<T,Args...>>
	{
		static constexpr uint32_t value = detail::MatchIndexV<T,v, Args...>::value();
	};


	template<typename T, template<typename,T...>typename Pack,T...Args>
	struct PackSize<Pack<T,Args...>>
	{
		static constexpr size_t value = sizeof...(Args);
	};

	template<size_t index,typename T, template<typename ,T...>typename Pack, T...Args>
	struct Get<index,Pack<T,Args...>>
	{
		static constexpr T value()
		{
			return Count< index, Args...>::value;
		}
	};
}