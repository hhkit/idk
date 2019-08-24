#pragma once
#include <compare>

namespace idk::math
{
	// forward declaration
	template <typename T> struct radian;
	template <typename T> struct degree;

	template <typename T>
	struct radian
	{
		T value;

		explicit radian(T val = T{});
		explicit radian(const degree<T>&);

		radian& normalize();        // normalize to (-pi, pi]
		radian  normalized() const; // normalize to (-pi, pi]

		T*       data();
		const T* data() const;

		// operator overloads
		explicit operator degree<T>() const;
		explicit operator T() const;

		radian& operator+=(const radian&);
		radian& operator-=(const radian&);
		radian& operator*=(const T&);
		radian& operator/=(const T&);

		radian operator+(const radian&) const;
		radian operator-(const radian&) const;
		radian operator*(const T&) const;
		radian operator/(const T&) const;
		T      operator/(const radian&) const;

		bool operator==(const radian&) const;
		bool operator!=(const radian&) const;
	};

	template<typename T>
	radian<T> operator*(const T&, const radian<T>&);

	template<typename T>
	struct degree
	{
		T value;

		explicit degree(T val = T{});
		explicit degree(const radian<T>&);

		degree& normalize();        // normalize to (-180, 180]
		degree  normalized() const; // normalize to (-180, 180]

		T*       data();
		const T* data() const;

		operator radian<T>() const;
		explicit operator T() const;

		degree& operator+=(const degree&);
		degree& operator-=(const degree&);
		degree& operator*=(const T&);
		degree& operator/=(const T&);

		degree operator+(const degree&) const;
		degree operator-(const degree&) const;
		degree operator*(const T&) const;
		degree operator/(const T&) const;
		T      operator/(const degree&) const;

		bool operator==(const degree&) const;
		bool operator!=(const degree&) const;
	};

	template<typename T>
	degree<T> operator*(const T&, const degree<T>&);
}

namespace idk
{
	template<typename T> auto cos(const math::radian<T>& r);
	template<typename T> auto sin(const math::radian<T>& r);
	template<typename T> auto tan(const math::radian<T>& r);
	template<typename T> auto cos(const math::degree<T>& d);
	template<typename T> auto sin(const math::degree<T>& d);
	template<typename T> auto tan(const math::degree<T>& d);
	template<typename T> math::radian<T> acos(const T& frac);
	template<typename T> math::radian<T> asin(const T& frac);
	template<typename T> math::radian<T> atan(const T& frac);
	template<typename T> math::radian<T> atan(const T& y, const T& x);
}

#include "angle.inl"