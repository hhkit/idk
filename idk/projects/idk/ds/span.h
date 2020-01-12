#pragma once
#include <utility>
namespace idk
{
	namespace detail
	{
		template<typename T, typename = void>
		struct is_contiguous_container : std::false_type {};

		template<typename T>
		struct is_contiguous_container<T, std::void_t<decltype(
			std::data(std::declval<T&>()),
			std::size(std::declval<T&>())
			)>> : std::true_type {};
		template<typename T> constexpr auto is_contiguous_container_v = is_contiguous_container<T>::value;
	}

	template <typename T>
	struct span
	{
		static constexpr auto npos = static_cast<size_t>(-1);
		using element_type = T;
		using value_type = std::remove_cv_t<T>;
		using size_t = std::size_t;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using iterator = T*;
		using const_iterator = const T*;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		T* _begin = nullptr;
		T* _end = nullptr;

		constexpr span() noexcept = default;
		constexpr span(T* begin, T* end) noexcept;
		template <typename Container, typename = std::enable_if_t<detail::is_contiguous_container_v<Container>
			&& std::is_convertible_v<decltype(std::data(std::declval<Container&>())), T*>
				&& !std::is_same_v<std::decay_t<Container>, span>>>
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
