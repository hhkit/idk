#pragma once
#include <utility>
#include <tuple>

#include "Vector_detail.h"
#include "Vector.h"

namespace idk::math
{
	template<typename T, unsigned D>
	template<typename ...Args, typename>
	constexpr vector<T, D>::vector(const Args& ... args)
		: vector{ detail::VectorConcat<T>(args...) }
	{
	}

	template<typename T, unsigned D>
	template<unsigned D2, typename>
	constexpr vector<T, D>::vector(const vector<T, D2>& rhs)
		: vector{ detail::TupleToVector<T>(detail::VectorToTuple<T>(rhs, std::make_index_sequence<D>{}), std::make_index_sequence<D>{}) }
	{
	}

	template<typename T, unsigned D>
	T vector<T, D>::length_sq() const
	{
		return dot(*this);
	}

	template<typename T, unsigned D>
	T vector<T, D>::length() const
	{
		if constexpr (std::is_same_v<float, T>)
			return sqrtf(length_sq());
		else
			return sqrt(static_cast<double>(length_sq()));
	}

	template<typename T, unsigned D>
	T vector<T, D>::distance_sq(const vector& rhs) const
	{
		return (rhs - *this).length_sq();
	}

	template<typename T, unsigned D>
	 T vector<T, D>::distance(const vector& rhs) const
	{
		 if constexpr (std::is_same_v<float, T>)
			 return sqrtf(distance_sq(rhs));
		 else
			 return sqrt(static_cast<double>(distance_sq(rhs)));
	}

	template<typename T, unsigned D>
	T vector<T, D>::dot(const vector& rhs) const
	{
		T accum{};
		for (auto& elem : *this * rhs)
			accum += elem;
		return accum;
	}

	template<typename T, unsigned D>
	vector<T, D>& vector<T, D>::normalize()
	{
		auto mag = length();

		for (auto& elem : *this)
			elem /= mag;

		return *this;
	}

	template<typename T, unsigned D>
	vector<T, D> vector<T, D>::get_normalized() const
	{
		auto copy = *this;
		return copy.normalize();
	}

	template<typename T, unsigned D>
	inline T* vector<T, D>::data()
	{
		return begin();
	}

	template<typename T, unsigned D>
	inline const T* vector<T, D>::data() const
	{
		return begin();
	}

	template<typename T, unsigned D>
	vector<T, D>& vector<T, D>::operator+=(const vector& rhs)
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			*ltr++ += *rtr++;

		return *this;
	}

	template<typename T, unsigned D>
	vector<T, D> vector<T, D>::operator+(const vector& rhs) const
	{
		auto copy = *this;
		return copy += rhs;
	}

	template<typename T, unsigned D>
	vector<T, D>& vector<T, D>::operator-=(const vector& rhs)
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			* ltr++ -= *rtr++;

		return *this;
	}
	template<typename T, unsigned D>
	vector<T, D> vector<T, D>::operator-() const
	{
		auto copy = *this;
		for (auto& elem : copy)
			elem = -elem;
		return copy;
	}

	template<typename T, unsigned D>
	vector<T, D> vector<T, D>::operator-(const vector& rhs) const
	{
		auto copy = *this;
		return copy -= rhs;
	}

	template<typename T, unsigned D>
	vector<T,D>& vector<T, D>::operator*=(const vector& rhs)
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			*ltr++ *= *rtr++;
		return *this;
	}

	template<typename T, unsigned D>
	vector<T,D> vector<T, D>::operator*(const vector& rhs) const
	{
		auto copy = *this;
		return copy *= rhs;
	}

	template<typename T, unsigned D>
	vector<T, D>& vector<T, D>::operator *= (const T& coeff)
	{
		for (auto& elem : *this)
			elem *= coeff;
		return *this;
	}

	template<typename T, unsigned D>
	vector<T, D> vector<T, D>::operator * (const T& coeff) const
	{
		auto copy = *this;
		return copy *= coeff;
	}

	template<typename T, unsigned D>
	vector<T, D>& vector<T, D>::operator/=(const vector& rhs)
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			* ltr++ /= *rtr++;
		return *this;
	}

	template<typename T, unsigned D>
	vector<T, D> vector<T, D>::operator/(const vector& rhs) const
	{
		auto copy = *this;
		return copy /= rhs;
	}


	template<typename T, unsigned D>
	vector<T, D>& vector<T, D>::operator /= (const T& coeff)
	{
		for (auto& elem : *this)
			elem /= coeff;
		return *this;
	}

	template<typename T, unsigned D>
	vector<T, D> vector<T, D>::operator / (const T& coeff) const
	{
		auto copy = *this;
		return copy /= coeff;
	}


	template<typename T, unsigned D>
	inline bool vector<T, D>::operator==(const vector& rhs) const
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
	inline bool vector<T, D>::operator!=(const vector& rhs) const
	{
		return !operator==(rhs);
	}

	template<typename T, unsigned D>
	T& vector<T, D>::operator[](unsigned i)
	{
		return data()[i];
	}

	template<typename T, unsigned D>
	const T& vector<T, D>::operator[](unsigned i) const
	{
		return data()[i];
	}
}