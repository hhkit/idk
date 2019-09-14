#pragma once

namespace idk
{
	template <class T>
	constexpr inline void hash_combine(size_t& seed, const T& v) noexcept
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
}