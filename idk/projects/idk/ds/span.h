#pragma once
#include <utility>
namespace idk
{
	template <typename T>
	struct span
	{
		static constexpr auto npos = static_cast<size_t>(-1);

		T* _begin = nullptr;
		T* _end = nullptr;

		constexpr span() noexcept = default;
		constexpr span(T* begin, T* end) noexcept;
		template <typename Container, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Container>, span>>>
		constexpr span(Container&& c) noexcept;

		// element access
		constexpr T& operator[](size_t index) const noexcept;
		constexpr T& front() const noexcept;
		constexpr T& back() const noexcept;
		constexpr T* data() const noexcept;

		// iterators
		constexpr T* begin() const noexcept;
		constexpr T* end() const noexcept;

		// subviews
		constexpr span<T> subspan(size_t offset, size_t count = npos) const noexcept;

		// capacity
		constexpr size_t size() const noexcept;
		constexpr size_t size_bytes() const noexcept;
		constexpr bool empty() const noexcept;
	};
}
