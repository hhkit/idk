#include "pch.h"
#pragma once
#include <iosfwd>
#include <idk.h>

namespace idk::vkn::hlp {

	std::ostream& cerr();

	template<typename K, typename V>
	hash_table<V, K> ReverseMap(const hash_table<K, V>& map);
	std::string GetBinaryFile(const std::string& filepath);
}
#include "utils.inl"