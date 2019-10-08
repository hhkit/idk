#pragma once
#include <math/comparable.h>

namespace idk
{
	// forward declaration
	template <typename T> struct trad;
	template <typename T> struct tdeg;

	template <typename T>
	struct trad : comparable<trad<T>>
	{
		T value;

		constexpr explicit trad(T val = T{}) noexcept;
		constexpr explicit trad(const tdeg<T>&) noexcept;

		constexpr trad& normalize();        // normalize to (-pi, pi]
		constexpr trad  normalized() const; // normalize to (-pi, pi]

		T*       data();
		const T* data() const;

		// operator overloads
		constexpr explicit operator tdeg<T>() const;
		constexpr explicit operator T() const;

		constexpr trad& operator+=(const trad&);
		constexpr trad& operator-=(const trad&);
		constexpr trad& operator*=(const T&);
		constexpr trad& operator/=(const T&);

		constexpr trad operator+(const trad&) const;
		constexpr trad operator-(const trad&) const;
		constexpr trad operator-() const;
		constexpr trad operator*(const T&) const;
		constexpr trad operator/(const T&) const;
		constexpr T      operator/(const trad&) const;

		constexpr bool operator<(const trad&) const;

		constexpr bool operator==(const trad&) const;
		constexpr bool operator!=(const trad&) const;
	};

	template<typename T>
	constexpr trad<T> operator*(const T&, const trad<T>&);

	template<typename T>
	struct tdeg : comparable<tdeg<T>>
	{
		T value;

		constexpr explicit tdeg(T val = T{}) noexcept;
		constexpr explicit tdeg(const trad<T>&) noexcept;

		constexpr tdeg& normalize();        // normalize to (-180, 180]
		constexpr tdeg  normalized() const; // normalize to (-180, 180]

		T*       data();
		const T* data() const;

		constexpr operator trad<T>() const;
		constexpr explicit operator T() const;

		constexpr tdeg& operator+=(const tdeg&);
		constexpr tdeg& operator-=(const tdeg&);
		constexpr tdeg& operator*=(const T&);
		constexpr tdeg& operator/=(const T&);

		constexpr tdeg operator+(const tdeg&) const;
		constexpr tdeg operator-(const tdeg&) const;
		constexpr tdeg operator-() const;
		constexpr tdeg operator*(const T&) const;
		constexpr tdeg operator/(const T&) const;
		constexpr T      operator/(const tdeg&) const;

		constexpr bool operator<(const tdeg&) const;

		constexpr bool operator==(const tdeg&) const;
		constexpr bool operator!=(const tdeg&) const;
	};

	template<typename T>
	constexpr tdeg<T> operator*(const T&, const tdeg<T>&);
}

namespace idk
{
	template<typename T> auto cos(const trad<T>& r);
	template<typename T> auto sin(const trad<T>& r);
	template<typename T> auto tan(const trad<T>& r);
	template<typename T> auto cos(const tdeg<T>& d);
	template<typename T> auto sin(const tdeg<T>& d);
	template<typename T> auto tan(const tdeg<T>& d);
	template<typename T> trad<T> acos(const T& frac);
	template<typename T> trad<T> asin(const T& frac);
	template<typename T> trad<T> atan(const T& frac);
	template<typename T> trad<T> atan(const T& y, const T& x);
}

#include "angle.inl"