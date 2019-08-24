#pragma once
#include <string_view>

namespace idk
{
	constexpr inline size_t string_hash(std::string_view str) noexcept
	{
		constexpr size_t fnv_offset_basis = 0xcbf29ce484222325;
		constexpr size_t fnv_prime = 0x100000001b3;

		auto hash = fnv_offset_basis;
		for (auto& elem : str)
		{
			hash *= fnv_prime;
			hash ^= elem;
		}
		hash *= fnv_prime;
		hash ^= 0;

		return hash;
	}
}