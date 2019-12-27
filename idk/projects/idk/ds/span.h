#pragma once
#include <utility>

namespace idk
{
	template <typename T>
	struct span
	{
		T* _begin = nullptr;
		T* _end = nullptr;

		constexpr span() noexcept = default;
		constexpr span(T* begin, T* end) noexcept;
		template <typename Container, typename = std::enable_if_t<!std::is_same_v<Container, span>>>
		constexpr explicit span(Container& c) noexcept;
		constexpr T* begin() const noexcept;
		constexpr T* end() const noexcept;
		constexpr size_t size() const noexcept;
		constexpr T& operator[](size_t index) const noexcept;
	};
}
