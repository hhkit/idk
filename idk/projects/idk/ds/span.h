#pragma once
namespace idk
{
	template <typename T>
	struct span
	{
		T* _begin;
		T* _end;

		constexpr span(T* begin, T* end) noexcept;
		template <typename Container, typename = sfinae<!std::is_same_v<Container, span>>>
		constexpr explicit span(Container& c) noexcept;
		constexpr T* begin() const noexcept;
		constexpr T* end() const noexcept;
		constexpr size_t size() const noexcept;
		constexpr T& operator[](size_t index) const noexcept;
	};
}

#include "span.inl"