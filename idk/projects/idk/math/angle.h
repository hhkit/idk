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

		explicit trad(T val = T{});
		explicit trad(const tdeg<T>&);

		trad& normalize();        // normalize to (-pi, pi]
		trad  normalized() const; // normalize to (-pi, pi]

		T*       data();
		const T* data() const;

		// operator overloads
		explicit operator tdeg<T>() const;
		explicit operator T() const;

		trad& operator+=(const trad&);
		trad& operator-=(const trad&);
		trad& operator*=(const T&);
		trad& operator/=(const T&);

		trad operator+(const trad&) const;
		trad operator-(const trad&) const;
		trad operator*(const T&) const;
		trad operator/(const T&) const;
		T      operator/(const trad&) const;

		bool operator<(const trad&) const;

		bool operator==(const trad&) const;
		bool operator!=(const trad&) const;
	};

	template<typename T>
	trad<T> operator*(const T&, const trad<T>&);

	template<typename T>
	struct tdeg : comparable<tdeg<T>>
	{
		T value;

		explicit tdeg(T val = T{});
		explicit tdeg(const trad<T>&);

		tdeg& normalize();        // normalize to (-180, 180]
		tdeg  normalized() const; // normalize to (-180, 180]

		T*       data();
		const T* data() const;

		operator trad<T>() const;
		explicit operator T() const;

		tdeg& operator+=(const tdeg&);
		tdeg& operator-=(const tdeg&);
		tdeg& operator*=(const T&);
		tdeg& operator/=(const T&);

		tdeg operator+(const tdeg&) const;
		tdeg operator-(const tdeg&) const;
		tdeg operator*(const T&) const;
		tdeg operator/(const T&) const;
		T      operator/(const tdeg&) const;

		bool operator<(const tdeg&) const;

		bool operator==(const tdeg&) const;
		bool operator!=(const tdeg&) const;
	};

	template<typename T>
	tdeg<T> operator*(const T&, const tdeg<T>&);
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