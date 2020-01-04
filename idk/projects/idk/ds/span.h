#pragma once
#include <utility>
#include <iterator>
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
		template<typename U,typename = std::enable_if_t<std::is_convertible_v<U(*)[],T(*)[]>>>
		span(span<U> other) : span{other._begin,other._end}{}
		constexpr T* begin() const noexcept;
		constexpr T* end() const noexcept;
		constexpr size_t size() const noexcept;
		constexpr T& operator[](size_t index) const noexcept;
	};
	//User defined deduction guide (C++17 onwards)
	template<typename Container>
	explicit span(Container&)-> span < typename std::iterator_traits<decltype(std::declval<Container>().data())>::value_type>;
}
