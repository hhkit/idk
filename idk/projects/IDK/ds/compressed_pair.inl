#pragma once

#include "compressed_pair.h"

namespace idk
{
	namespace detail
	{
		template<typename T, size_t I>
		struct compressed_pair_elem<T, I, false> // base class NOT empty
		{
			constexpr compressed_pair_elem() noexcept(std::is_nothrow_default_constructible<T>::value)
				: _value{}
			{
				static_assert(!std::is_reference<T>::value,
					"attempted to default construct a reference element");
			}

			template<typename U, typename = std::enable_if_t<std::is_constructible_v<T, U>>>
			constexpr explicit compressed_pair_elem(U&& other) noexcept(std::is_nothrow_constructible_v<T, U>)
				: _value(std::forward<U>(other))
			{}

			constexpr T& get() noexcept
			{
				return _value;
			}

			constexpr const T& get() const noexcept
			{
				return _value;
			}

		private:
			T _value;
		};

		template<typename T, size_t I>
		struct compressed_pair_elem<T, I, true> : private T // base class empty
		{
			constexpr compressed_pair_elem() noexcept(std::is_nothrow_default_constructible<T>::value)
			{
			}

			template<typename U, typename = std::enable_if_t<std::is_constructible_v<T, U>>>
			constexpr explicit compressed_pair_elem(U&& other) noexcept(std::is_nothrow_constructible_v<T, U>)
				: T(std::forward<U>(other))
			{}

			constexpr T& get() noexcept
			{
				return *this;
			}

			constexpr const T& get() const noexcept
			{
				return *this;
			}
		};
	}

	template<typename T1, typename T2>
	constexpr compressed_pair<T1, T2>::compressed_pair() noexcept(
		std::is_nothrow_default_constructible_v<T1> &&
		std::is_nothrow_default_constructible_v<T2>)
		: detail::compressed_pair_elem<T1, 0>(), detail::compressed_pair_elem<T2, 1>()
	{}

	template<typename T1, typename T2>
	template<typename U1, typename U2, typename>
	constexpr compressed_pair<T1, T2>::compressed_pair(U1&& first, U2&& second) noexcept(
		std::is_nothrow_constructible_v<T1, U1>&&
		std::is_nothrow_constructible_v<T2, U2>)
		: detail::compressed_pair_elem<T1, 0>(std::forward<U1>(first)), detail::compressed_pair_elem<T2, 1>(std::forward<U2>(second))
	{}

	template<typename T1, typename T2>
	constexpr T1& compressed_pair<T1, T2>::first() noexcept
	{
		return detail::compressed_pair_elem<T1, 0>::get();
	}

	template<typename T1, typename T2>
	constexpr const T1& compressed_pair<T1, T2>::first() const noexcept
	{
		return detail::compressed_pair_elem<T1, 0>::get();
	}

	template<typename T1, typename T2>
	constexpr T2& compressed_pair<T1, T2>::second() noexcept
	{
		return detail::compressed_pair_elem<T2, 1>::get();
	}

	template<typename T1, typename T2>
	constexpr const T2& compressed_pair<T1, T2>::second() const noexcept
	{
		return detail::compressed_pair_elem<T2, 1>::get();
	}
}