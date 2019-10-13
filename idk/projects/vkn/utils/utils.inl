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
	}

	template<typename ...Args, typename T>
	struct IndexOf< variant<Args...>, T>
	{
		static constexpr uint32_t value = detail::MatchIndex<T, Args...>::value();
	};
}