#pragma once

#include "constants.h"
#include "angle.h"
#include "arith.h"
#include "../meta/casts.h"
#include <cmath>
namespace idk
{
	template<typename T>
	inline trad<T>::trad(T val)
		: value{ val }
	{
	}
	template<typename T>
	inline trad<T>::trad(const tdeg<T>& rhs)
		: value{ rhs.value / s_cast<T>(180) * constants::pi<T>()}
	{
	}

	template<typename T>
	trad<T>& trad<T>::normalize()
	{
		value = fmod(value, constants::tau<T>());
		if (value > constants::pi<T>())
			value -= constants::tau<T>();
		return *this;
	}

	template<typename T>
	trad<T> trad<T>::normalized() const
	{
		auto copy = *this;
		return copy.normalize();
	}

	template<typename T>
	T* trad<T>::data()
	{
		return &value;
	}

	template<typename T>
	const T* trad<T>::data() const
	{
		return &value;
	}

	template<typename T>
	inline trad<T>::operator T() const
	{
		return value;
	}

	template<typename T>
	trad<T>& trad<T>::operator+=(const trad& rhs)
	{
		value += rhs.value;
		return *this;
	}

	template<typename T>
	trad<T>& trad<T>::operator-=(const trad& rhs)
	{
		value -= rhs.value;
		return *this;
	}

	template<typename T>
	trad<T>& trad<T>::operator*=(const T& rhs)
	{
		value *= rhs;
		return *this;
	}

	template<typename T>
	trad<T>& trad<T>::operator/=(const T& rhs)
	{
		value /= rhs;
		return *this;
	}

	template<typename T>
	inline trad<T> trad<T>::operator+(const trad& rhs) const
	{
		auto copy = *this;
		return copy += rhs;
	}

	template<typename T>
	inline trad<T> trad<T>::operator-(const trad& rhs) const
	{
		auto copy = *this;
		return copy -= rhs;
	}

	template<typename T>
	inline trad<T> trad<T>::operator-() const
	{
		return trad<T>{-this->value};
	}

	template<typename T>
	inline trad<T> trad<T>::operator*(const T& rhs) const
	{
		auto copy = *this;
		return copy *= rhs;
	}

	template<typename T>
	inline trad<T> trad<T>::operator/(const T& rhs) const
	{
		auto copy = *this;
		return copy /= rhs;
	}

	template<typename T>
	inline T trad<T>::operator/(const trad& rhs) const
	{
		return value / rhs.value;
	}

	template<typename T>
	inline bool trad<T>::operator<(const trad& rhs) const
	{
		return value < rhs.value;
	}

	template<typename T>
	inline bool trad<T>::operator==(const trad& rhs) const
	{
		return value == rhs.value;
	}

	template<typename T>
	inline bool trad<T>::operator!=(const trad& rhs) const
	{
		return value != rhs.value;
	}


	template<typename T>
	trad<T>::operator tdeg<T>() const
	{
		return tdeg<T>{value / constants::pi<T>() * s_cast<T>(180)};
	}


	template<typename T>
	inline tdeg<T>::tdeg(T val)
		: value{ val }
	{
	}
	template<typename T>
	inline tdeg<T>::tdeg(const trad<T>& rhs)
		: value{ rhs.value / constants::pi<T>() * s_cast<T>(180) }
	{
	}


	template<typename T>
	tdeg<T>& tdeg<T>::normalize()
	{
		value = fmod(value, T{ 360 });
		if (value > T{ 180 })
			value -= 360;
		return *this;
	}

	template<typename T>
	tdeg<T> tdeg<T>::normalized() const
	{
		auto copy = *this;
		return copy.normalize();
	}


	template<typename T>
	inline T* tdeg<T>::data()
	{
		return &value;
	}

	template<typename T>
	inline const T* tdeg<T>::data() const
	{
		return &value;
	}


	template<typename T>
	tdeg<T>::operator T() const
	{
		return value;
	}

	template<typename T>
	tdeg<T>& tdeg<T>::operator+=(const tdeg& rhs)
	{
		value += rhs.value;
		return *this;
	}

	template<typename T>
	tdeg<T>& tdeg<T>::operator-=(const tdeg& rhs)
	{
		value -= rhs.value;
		return *this;
	}

	template<typename T>
	tdeg<T>& tdeg<T>::operator*=(const T& rhs)
	{
		value *= rhs;
		return *this;
	}

	template<typename T>
	tdeg<T>& tdeg<T>::operator/=(const T& rhs)
	{
		value /= rhs;
		return *this;
	}

	template<typename T>
	inline tdeg<T> tdeg<T>::operator+(const tdeg& rhs) const
	{
		auto copy = *this;
		return copy += rhs;
	}

	template<typename T>
	inline tdeg<T> tdeg<T>::operator-(const tdeg& rhs) const
	{
		auto copy = *this;
		return copy -= rhs;
	}

	template<typename T>
	inline tdeg<T> tdeg<T>::operator-() const
	{
		return tdeg<T>{-this->value};
	}

	template<typename T>
	inline tdeg<T> tdeg<T>::operator*(const T& rhs) const
	{
		auto copy = *this;
		return copy *= rhs;
	}

	template<typename T>
	inline tdeg<T> tdeg<T>::operator/(const T& rhs) const
	{
		auto copy = *this;
		return copy /= rhs;
	}

	template<typename T>
	inline T tdeg<T>::operator/(const tdeg& rhs) const
	{
		return value / rhs.value;
	}

	template<typename T>
	inline bool tdeg<T>::operator<(const tdeg& rhs) const
	{
		return value < rhs.value;
	}

	template<typename T>
	inline bool tdeg<T>::operator==(const tdeg& rhs) const
	{
		return value == rhs.value;
	}

	template<typename T>
	inline bool tdeg<T>::operator!=(const tdeg& rhs) const
	{
		return value != rhs.value;
	}

	template<typename T>
	tdeg<T>::operator trad<T>() const
	{
		return trad<T>(value * constants::pi<T>() / s_cast<T>(180));
	}

	template<typename T>
	trad<T> operator*(const T& coeff, const trad<T>& r)
	{
		return r * coeff;
	}

	template<typename T>
	tdeg<T> operator*(const T& coeff, const tdeg<T>& d)
	{
		return d * coeff;
	}
}

namespace idk
{
	template<typename T>
	auto cos(const trad<T>& r)
	{
		return std::cos(r.value);
	}
	template<typename T>
	auto sin(const trad<T>& r)
	{
		return std::sin(r.value);
	}
	template<typename T>
	auto tan(const trad<T>& r)
	{
		return std::tan(r.value);
	}
	template<typename T>
	auto cos(const tdeg<T>& d)
	{
		return cos(trad<T>{ d });
	}
	template<typename T>
	auto sin(const tdeg<T>& d)
	{
		return sin(trad<T>{ d });
	}
	template<typename T>
	auto tan(const tdeg<T>& d)
	{
		return tan(trad<T>{ d });
	}
	template<typename T>
	trad<T> acos(const T& frac)
	{
		return trad<T>(std::acos(frac));
	}
	template<typename T>
	trad<T> asin(const T& frac)
	{
		return trad<T>(std::asin(frac));
	}
	template<typename T>
	trad<T> atan(const T& frac)
	{
		return trad<T>(std::atan(frac));
	}
	template<typename T>
	trad<T> atan(const T& y, const T& x)
	{
		return trad<T>(std::atan2(y, x));
	}
}