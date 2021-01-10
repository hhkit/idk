#pragma once
#include <utility>
#include <iterator>
#include "span.h"
namespace idk
{
	//namespace detail
	//{
	//	template<typename T, typename = void>
	//	struct is_contiguous_container : std::false_type {};
	//
	//	template<typename T>
	//	struct is_contiguous_container<T, std::void_t<decltype(
	//		std::data(std::declval<T&>()),
	//		std::size(std::declval<T&>())
	//		)>> : std::true_type {};
	//	template<typename T> constexpr auto is_contiguous_container_v = is_contiguous_container<T>::value;
	//}
	template<typename T>
	struct StrideItr
	{
		using element_type = T;
		using value_type = std::remove_cv_t<T>;
		using size_t = std::size_t;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		T* ptr{};
		size_t offset{};

		constexpr T* operator->()const noexcept { return ptr; }
		constexpr T& operator*()const noexcept{ return*ptr; }

		StrideItr& operator+=(difference_type index)
		{
			ptr += index;
			ptr = reinterpret_cast<T*>(reinterpret_cast<std::conditional_t<std::is_const_v<T>,const std::byte,std::byte>*>(ptr) + offset * index);
			return *this;
		}
		constexpr StrideItr operator+(difference_type index)const noexcept
		{
			StrideItr cpy = *this;
			cpy += index;
			return cpy;
		}

		StrideItr& operator++()
		{
			return (*this) += 1;
		}
		StrideItr operator++(int)
		{
			auto cpy = *this;
			++cpy;
			return cpy;
		}
		StrideItr& operator-=(difference_type index)
		{
			return *this += -index;
		}
		constexpr StrideItr operator-(difference_type index)const noexcept
		{
			StrideItr cpy = *this;
			cpy -= index;
			return cpy;
		}

		StrideItr& operator--()
		{
			return (*this) -= 1;
		}
		StrideItr operator--(int)
		{
			auto cpy = *this;
			--cpy;
			return cpy;
		}
	};
	template<typename T>
	bool operator==(const StrideItr<T>& lhs, const StrideItr<T>& rhs)noexcept
	{
		return  lhs.ptr == rhs.ptr;
	}
	template<typename T>
	bool operator!=(const StrideItr<T>& lhs, const StrideItr<T>& rhs)noexcept
	{
		return  lhs.ptr != rhs.ptr;
	}
	template<typename T>
	bool operator<(const StrideItr<T>& lhs, const StrideItr<T>& rhs)noexcept
	{
		return  lhs.ptr < rhs.ptr;
	}
	template<typename T>
	bool operator>(const StrideItr<T>& lhs, const StrideItr<T>& rhs)noexcept
	{
		return  lhs.ptr > rhs.ptr;
	}
	template <typename T>
	struct strided_span
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
		using iterator = StrideItr<T>;
		using const_iterator = StrideItr<const T>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;


		T* _begin = nullptr;
		T* _end = nullptr;
		size_t _offset;

		constexpr strided_span() noexcept = default;
		constexpr strided_span(T* begin, T* end, size_t offset = 0) noexcept;
		template<
			typename Derived, typename = std::enable_if_t<std::is_base_of_v<T,Derived>>
		>
		constexpr strided_span(Derived* begin, Derived* end,size_t offset=0) noexcept;
		template <typename Container, typename = std::enable_if_t<detail::is_contiguous_container_v<Container>
			&& std::is_convertible_v<decltype(std::data(std::declval<Container&>())), T*>
				&& !std::is_same_v<std::decay_t<Container>, strided_span>>>
				constexpr strided_span(Container&& c) noexcept;
			constexpr strided_span(const strided_span&) noexcept = default;

			// element access
			constexpr T& operator[](size_t index) const noexcept;
			constexpr T& front() const noexcept;
			constexpr T& back() const noexcept;
			constexpr T* data() const noexcept;

			// iterators
			constexpr iterator begin() const noexcept;
			constexpr iterator end() const noexcept;

			// subviews
			constexpr strided_span<T> subspan(size_t offset, size_t count = npos) const noexcept;

			// capacity
			constexpr size_t size() const noexcept;
			constexpr size_t size_bytes() const noexcept;
			constexpr bool empty() const noexcept;

			constexpr iterator get_iterator(T* ptr)const noexcept;
	};
	//User defined deduction guide (C++17 onwards)
	template<typename Container>
	strided_span(Container&&)->strided_span < std::remove_reference_t<decltype(*std::declval<Container>().data())>>;
}
