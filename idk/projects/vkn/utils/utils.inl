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