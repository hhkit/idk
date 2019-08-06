#pragma once

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
		explicit operator degree<T>() const;

		bool abs_comp(const radian&) const;

		// operator overloads
		radian& operator+=(const radian&);
		radian& operator-=(const radian&);
		radian& operator*=(const T&);
		radian& operator/=(const T&);

		radian operator+(const radian&) const;
		radian operator-(const radian&) const;
		radian operator*(const T&) const;
		radian operator/(const T&) const;

		bool operator==(const radian&) const;
		bool operator!=(const radian&) const;
	};

	template<typename T>
	struct degree
	{
		T value;

		explicit degree(T val = T{});
		explicit degree(const radian<T>&);
		operator radian<T>() const;

		bool abs_comp(const degree&) const;

		degree& operator+=(const degree&);
		degree& operator-=(const degree&);
		degree& operator*=(const T&);
		degree& operator/=(const T&);

		degree operator+(const degree&) const;
		degree operator-(const degree&) const;
		degree operator*(const T&) const;
		degree operator/(const T&) const;

		bool operator==(const degree&) const;
		bool operator!=(const degree&) const;
	};
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