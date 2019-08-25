#pragma once

#include "constants.h"
#include "angle.h"
#include "arith.h"
#include "../meta/casts.h"
#include <cmath>
namespace idk::math
{
	template<typename T>
	inline radian<T>::radian(T val)
		: value{ val }
	{
	}
	template<typename T>
	inline radian<T>::radian(const degree<T>& rhs)
		: value{ rhs.value / s_cast<T>(180) * constants::pi<T>()}
	{
	}

	template<typename T>
	radian<T>& radian<T>::normalize()
	{
		value = fmod(value, constants::tau<T>());
		if (value > constants::pi<T>())
			value -= constants::tau<T>();
		return *this;
	}

	template<typename T>
	radian<T> radian<T>::normalized() const
	{
		auto copy = *this;
		return copy.normalize();
	}

	template<typename T>
	T* radian<T>::data()
	{
		return &value;
	}

	template<typename T>
	const T* radian<T>::data() const
	{
		return &value;
	}

	template<typename T>
	inline radian<T>::operator T() const
	{
		return value;
	}

	template<typename T>
	radian<T>& radian<T>::operator+=(const radian& rhs)
	{
		value += rhs.value;
		return *this;
	}

	template<typename T>
	radian<T>& radian<T>::operator-=(const radian& rhs)
	{
		value -= rhs.value;
		return *this;
	}

	template<typename T>
	radian<T>& radian<T>::operator*=(const T& rhs)
	{
		value *= rhs;
		return *this;
	}

	template<typename T>
	radian<T>& radian<T>::operator/=(const T& rhs)
	{
		value /= rhs;
		return *this;
	}

	template<typename T>
	inline radian<T> radian<T>::operator+(const radian& rhs) const
	{
		auto copy = *this;
		return copy += rhs;
	}

	template<typename T>
	inline radian<T> radian<T>::operator-(const radian& rhs) const
	{
		auto copy = *this;
		return copy -= rhs;
	}

	template<typename T>
	inline radian<T> radian<T>::operator*(const T& rhs) const
	{
		auto copy = *this;
		return copy *= rhs;
	}

	template<typename T>
	inline radian<T> radian<T>::operator/(const T& rhs) const
	{
		auto copy = *this;
		return copy /= rhs;
	}

	template<typename T>
	inline T radian<T>::operator/(const radian& rhs) const
	{
		return value / rhs.value;
	}

	template<typename T>
	inline bool radian<T>::operator<(const radian& rhs) const
	{
		return value < rhs.value;
	}

	template<typename T>
	inline bool radian<T>::operator==(const radian& rhs) const
	{
		return value == rhs.value;
	}

	template<typename T>
	inline bool radian<T>::operator!=(const radian& rhs) const
	{
		return value != rhs.value;
	}


	template<typename T>
	radian<T>::operator degree<T>() const
	{
		return degree<T>{value / constants::pi<T>() * s_cast<T>(180)};
	}


	template<typename T>
	inline degree<T>::degree(T val)
		: value{ val }
	{
	}
	template<typename T>
	inline degree<T>::degree(const radian<T>& rhs)
		: value{ rhs.value / constants::pi<T>() * s_cast<T>(180) }
	{
	}


	template<typename T>
	degree<T>& degree<T>::normalize()
	{
		value = fmod(value, T{ 360 });
		if (value > T{ 180 })
			value -= 360;
		return *this;
	}

	template<typename T>
	degree<T> degree<T>::normalized() const
	{
		auto copy = *this;
		return copy.normalize();
	}


	template<typename T>
	inline T* degree<T>::data()
	{
		return &value;
	}

	template<typename T>
	inline const T* degree<T>::data() const
	{
		return &value;
	}


	template<typename T>
	degree<T>::operator T() const
	{
		return value;
	}

	template<typename T>
	degree<T>& degree<T>::operator+=(const degree& rhs)
	{
		value += rhs.value;
		return *this;
	}

	template<typename T>
	degree<T>& degree<T>::operator-=(const degree& rhs)
	{
		value -= rhs.value;
		return *this;
	}

	template<typename T>
	degree<T>& degree<T>::operator*=(const T& rhs)
	{
		value *= rhs;
		return *this;
	}

	template<typename T>
	degree<T>& degree<T>::operator/=(const T& rhs)
	{
		value /= rhs;
		return *this;
	}

	template<typename T>
	inline degree<T> degree<T>::operator+(const degree& rhs) const
	{
		auto copy = *this;
		return copy += rhs;
	}

	template<typename T>
	inline degree<T> degree<T>::operator-(const degree& rhs) const
	{
		auto copy = *this;
		return copy -= rhs;
	}

	template<typename T>
	inline degree<T> degree<T>::operator*(const T& rhs) const
	{
		auto copy = *this;
		return copy *= rhs;
	}

	template<typename T>
	inline degree<T> degree<T>::operator/(const T& rhs) const
	{
		auto copy = *this;
		return copy /= rhs;
	}

	template<typename T>
	inline T degree<T>::operator/(const degree& rhs) const
	{
		return value / rhs.value;
	}

	template<typename T>
	inline bool degree<T>::operator<(const degree& rhs) const
	{
		return value < rhs.value;
	}

	template<typename T>
	inline bool degree<T>::operator==(const degree& rhs) const
	{
		return value == rhs.value;
	}

	template<typename T>
	inline bool degree<T>::operator!=(const degree& rhs) const
	{
		return value != rhs.value;
	}

	template<typename T>
	degree<T>::operator radian<T>() const
	{
		return radian<T>(value * constants::pi<T>() / s_cast<T>(180));
	}

	template<typename T>
	radian<T> operator*(const T& coeff, const radian<T>& r)
	{
		return r * coeff;
	}

	template<typename T>
	degree<T> operator*(const T& coeff, const degree<T>& d)
	{
		return d * coeff;
	}
}

namespace idk
{
	template<typename T>
	auto cos(const math::radian<T>& r)
	{
		return std::cos(r.value);
	}
	template<typename T>
	auto sin(const math::radian<T>& r)
	{
		return std::sin(r.value);
	}
	template<typename T>
	auto tan(const math::radian<T>& r)
	{
		return std::tan(r.value);
	}
	template<typename T>
	auto cos(const math::degree<T>& d)
	{
		return cos(math::radian<T>{ d });
	}
	template<typename T>
	auto sin(const math::degree<T>& d)
	{
		return sin(math::radian<T>{ d });
	}
	template<typename T>
	auto tan(const math::degree<T>& d)
	{
		return tan(math::radian<T>{ d });
	}
	template<typename T>
	math::radian<T> acos(const T& frac)
	{
		return math::radian<T>(std::acos(frac));
	}
	template<typename T>
	math::radian<T> asin(const T& frac)
	{
		return math::radian<T>(std::acos(frac));
	}
	template<typename T>
	math::radian<T> atan(const T& frac)
	{
		return math::radian<T>(std::atan(frac));
	}
	template<typename T>
	math::radian<T> atan(const T& y, const T& x)
	{
		return math::radian<T>(std::atan2(y, x));
	}
}