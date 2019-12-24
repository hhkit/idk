#pragma once

#include <type_traits>
#include <memory>

namespace idk
{
	namespace detail
	{
		template<typename T, size_t I, bool = std::is_empty<T>::value>
		struct compressed_pair_elem;

        template<typename T, size_t I>
        struct compressed_pair_elem<T, I, false> // base class NOT empty
        {
            constexpr compressed_pair_elem() noexcept(std::is_nothrow_default_constructible<T>::value);

            template<typename U, typename = std::enable_if_t<std::is_constructible_v<T, U>>>
            constexpr explicit compressed_pair_elem(U&& other) noexcept(std::is_nothrow_constructible_v<T, U>);

            constexpr T& get() noexcept;
            constexpr const T& get() const noexcept;

        private:
            T _value;
        };

        template<typename T, size_t I>
        struct compressed_pair_elem<T, I, true> : private T // base class empty
        {
            constexpr compressed_pair_elem() noexcept(std::is_nothrow_default_constructible<T>::value);

            template<typename U, typename = std::enable_if_t<std::is_constructible_v<T, U>>>
            constexpr explicit compressed_pair_elem(U&& other) noexcept(std::is_nothrow_constructible_v<T, U>);

            constexpr T& get() noexcept;
            constexpr const T& get() const noexcept;
        };
	}

	// class to take advantage of empty base optimization
	template<typename T1, typename T2>
	struct compressed_pair :
		private detail::compressed_pair_elem<T1, 0>,
		private detail::compressed_pair_elem<T2, 1>
	{
		constexpr compressed_pair() noexcept(
			std::is_nothrow_default_constructible_v<T1> &&
			std::is_nothrow_default_constructible_v<T2>);

		template<typename U1 = T1, typename U2 = T2, typename = std::enable_if_t<
			std::is_constructible_v<T1, U1> && std::is_constructible_v<T2, U2>>>
		constexpr explicit compressed_pair(U1&& first, U2&& second) noexcept(
			std::is_nothrow_constructible_v<T1, U1> &&
			std::is_nothrow_constructible_v<T2, U2>);

		constexpr T1&       first() noexcept;
		constexpr const T1& first() const noexcept;
		constexpr T2&       second() noexcept;
		constexpr const T2& second() const noexcept;
	};
}
