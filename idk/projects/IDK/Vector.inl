#pragma once
#include <utility>
#include <numeric>
#include <tuple>
#include "Vector_detail.h"
#include "Vector.h"

namespace idk
{
	template<typename T, unsigned D>
	template<typename ...Args, typename, typename>
	inline Vector<T, D>::Vector(Args ... args)
		: Vector{ detail::VectorConcat<T>(args...) }
	{
	}

	template<typename T, unsigned D>
	T Vector<T, D>::magnitude_sq() const
	{
		T accum{};
		for (auto& elem : *this)
			accum += elem * elem;
		return accum;
	}

	template<typename T, unsigned D>
	T Vector<T, D>::magnitude() const
	{
		if constexpr (std::is_same_v<float, T>)
			return sqrtf(magnitude_sq());
		else
			return sqrt(static_cast<double>(magnitude_sq()));
	}

	template<typename T, unsigned D>
	T Vector<T, D>::distance_sq(const Vector& rhs) const
	{
		return (rhs - *this).magnitude_sq();
	}

	template<typename T, unsigned D>
	 T Vector<T, D>::distance(const Vector& rhs) const
	{
		 if constexpr (std::is_same_v<float, T>)
			 return sqrtf(distance_sq(rhs));
		 else
			 return sqrt(static_cast<double>(distance_sq(rhs)));
	}

	template<typename T, unsigned D>
	T Vector<T, D>::dot(const Vector& rhs) const
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		T accum{};
		while (ltr != etr)
			accum += *ltr++ * *rtr++;
		return accum;
	}

	template<typename T, unsigned D>
	Vector<T, D>& Vector<T, D>::normalize()
	{
		auto mag = magnitude();

		for (auto& elem : *this)
			elem /= mag;

		return *this;
	}

	template<typename T, unsigned D>
	Vector<T, D> Vector<T, D>::get_normalized() const
	{
		auto copy = *this;
		return copy.normalize();
	}

	template<typename T, unsigned D>
	inline T* Vector<T, D>::data()
	{
		return begin();
	}

	template<typename T, unsigned D>
	inline const T* Vector<T, D>::data() const
	{
		return begin();
	}

	template<typename T, unsigned D>
	Vector<T, D>& Vector<T, D>::operator+=(const Vector& rhs)
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			*ltr++ += *rtr++;

		return *this;
	}

	template<typename T, unsigned D>
	Vector<T, D> Vector<T, D>::operator+(const Vector& rhs) const
	{
		auto copy = *this;
		return copy += rhs;
	}

	template<typename T, unsigned D>
	Vector<T, D>& Vector<T, D>::operator-=(const Vector& rhs)
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			* ltr++ -= *rtr++;

		return *this;
	}
	template<typename T, unsigned D>
	Vector<T, D> Vector<T, D>::operator-() const
	{
		auto copy = *this;
		for (auto& elem : copy)
			elem = -elem;
		return copy;
	}

	template<typename T, unsigned D>
	Vector<T, D> Vector<T, D>::operator-(const Vector& rhs) const
	{
		auto copy = *this;
		return copy -= rhs;
	}

	template<typename T, unsigned D>
	template<typename U>
	Vector<T, D>& Vector<T, D>::operator *= (const U& coeff)
	{
		for (auto& elem : *this)
			elem *= coeff;
		return *this;
	}

	template<typename T, unsigned D>
	template<typename U>
	Vector<T, D> Vector<T, D>::operator * (const U& coeff) const
	{
		auto copy = *this;
		return copy *= coeff;
	}


	template<typename T, unsigned D>
	inline bool Vector<T, D>::operator==(const Vector& rhs) const
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			if (*ltr++ != *rtr++)
				return false;

		return true;
	}

	template<typename T, unsigned D>
	inline bool Vector<T, D>::operator!=(const Vector& rhs) const
	{
		return !operator==(rhs);
	}

	template<typename T, unsigned D>
	T& Vector<T, D>::operator[](unsigned i)
	{
		return data()[i];
	}

	template<typename T, unsigned D>
	const T& Vector<T, D>::operator[](unsigned i) const
	{
		return data()[i];
	}

}