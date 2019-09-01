#pragma once
#include <utility>
#include <tuple>

#include "Vector_detail.h"
#include "Vector.h"
#include <meta/tuple.h>

namespace idk
{
	template<typename T, unsigned D>
	inline constexpr tvec<T, D>::tvec(const T& fill)
	{
		for (auto& elem : *this)
			elem = fill;
	}

	template<typename T, unsigned D>
	template<typename ...Args, typename>
	constexpr tvec<T, D>::tvec(const Args& ... args)
		: tvec{ detail::VectorConcat<T>(args...) }
	{
	}

	template<typename T, unsigned D>
	template<unsigned D2, typename>
	constexpr tvec<T, D>::tvec(const tvec<T, D2>& rhs)
		: tvec{ tuple_construct<tvec>(detail::VectorToTuple<T>(rhs, std::make_index_sequence<D>{})) }
	{
	}

	template<typename T, unsigned D>
	T tvec<T, D>::length_sq() const
	{
		return dot(*this);
	}

	template<typename T, unsigned D>
	T tvec<T, D>::length() const
	{
		if constexpr (std::is_same_v<float, T>)
			return sqrtf(length_sq());
		else
			return sqrt(static_cast<double>(length_sq()));
	}

	template<typename T, unsigned D>
	T tvec<T, D>::distance_sq(const tvec& rhs) const
	{
		return (rhs - *this).length_sq();
	}

	template<typename T, unsigned D>
	 T tvec<T, D>::distance(const tvec& rhs) const
	{
		 if constexpr (std::is_same_v<float, T>)
			 return sqrtf(distance_sq(rhs));
		 else
			 return sqrt(static_cast<double>(distance_sq(rhs)));
	}

	template<typename T, unsigned D>
	T tvec<T, D>::dot(const tvec& rhs) const
	{
		T accum{};
		for (auto& elem : *this * rhs)
			accum += elem;
		return accum;
	}

	template<typename T, unsigned D>
	tvec<T, D> tvec<T, D>::project_onto(const tvec& rhs)const
	{
		return (dot(rhs) * rhs)/ rhs.length_sq();
	}

	template<typename T, unsigned D>
	tvec<T, D>& tvec<T, D>::normalize()
	{
		auto mag = length();

		for (auto& elem : *this)
			elem /= mag;

		return *this;
	}

	template<typename T, unsigned D>
	tvec<T, D> tvec<T, D>::get_normalized() const
	{
		auto copy = *this;
		return copy.normalize();
	}

	template<typename T, unsigned D>
	constexpr T* tvec<T, D>::begin() noexcept
	{
		return std::begin(values);
	}
	template<typename T, unsigned D>
	constexpr T* tvec<T, D>::end() noexcept
	{
		return std::end(values);
	}
	template<typename T, unsigned D>
	constexpr const T* tvec<T, D>::begin() const noexcept
	{
		return std::begin(values);
	}

	template<typename T, unsigned D>
	constexpr const T* tvec<T, D>::end() const noexcept
	{
		return std::end(values);
	}

	template<typename T, unsigned D>
	constexpr T* tvec<T, D>::data() noexcept
	{
		return begin();
	}

	template<typename T, unsigned D>
	constexpr const T* tvec<T, D>::data() const noexcept
	{
		return begin();
	}

	template<typename T, unsigned D>
	tvec<T, D>& tvec<T, D>::operator+=(const tvec& rhs)
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			*ltr++ += *rtr++;

		return *this;
	}

	template<typename T, unsigned D>
	tvec<T, D> tvec<T, D>::operator+(const tvec& rhs) const
	{
		auto copy = *this;
		return copy += rhs;
	}

	template<typename T, unsigned D>
	tvec<T, D>& tvec<T, D>::operator-=(const tvec& rhs)
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			* ltr++ -= *rtr++;

		return *this;
	}
	template<typename T, unsigned D>
	tvec<T, D> tvec<T, D>::operator-() const
	{
		auto copy = *this;
		for (auto& elem : copy)
			elem = -elem;
		return copy;
	}

	template<typename T, unsigned D>
	tvec<T, D> tvec<T, D>::operator-(const tvec& rhs) const
	{
		auto copy = *this;
		return copy -= rhs;
	}

	template<typename T, unsigned D>
	tvec<T,D>& tvec<T, D>::operator*=(const tvec& rhs)
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			*ltr++ *= *rtr++;
		return *this;
	}

	template<typename T, unsigned D>
	tvec<T,D> tvec<T, D>::operator*(const tvec& rhs) const
	{
		auto copy = *this;
		return copy *= rhs;
	}

	template<typename T, unsigned D>
	tvec<T, D>& tvec<T, D>::operator *= (const T& coeff)
	{
		for (auto& elem : *this)
			elem *= coeff;
		return *this;
	}

	template<typename T, unsigned D>
	tvec<T, D> tvec<T, D>::operator * (const T& coeff) const
	{
		auto copy = *this;
		return copy *= coeff;
	}

	template<typename T, unsigned D>
	tvec<T, D>& tvec<T, D>::operator/=(const tvec& rhs)
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			* ltr++ /= *rtr++;
		return *this;
	}

	template<typename T, unsigned D>
	tvec<T, D> tvec<T, D>::operator/(const tvec& rhs) const
	{
		auto copy = *this;
		return copy /= rhs;
	}


	template<typename T, unsigned D>
	tvec<T, D>& tvec<T, D>::operator /= (const T& coeff)
	{
		for (auto& elem : *this)
			elem /= coeff;
		return *this;
	}

	template<typename T, unsigned D>
	tvec<T, D> tvec<T, D>::operator / (const T& coeff) const
	{
		auto copy = *this;
		return copy /= coeff;
	}


	template<typename T, unsigned D>
	inline bool tvec<T, D>::operator==(const tvec& rhs) const
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
	inline bool tvec<T, D>::operator!=(const tvec& rhs) const
	{
		return !operator==(rhs);
	}

	template<typename T, unsigned D>
	constexpr T& tvec<T, D>::operator[](size_t i) noexcept
	{
		return data()[i];
	}

	template<typename T, unsigned D>
	constexpr const T& tvec<T, D>::operator[](size_t i) const noexcept
	{
		return data()[i];
	}

	template<typename T, unsigned D>
	tvec<T, D> operator*(const T& coeff, const tvec<T, D>& v)
	{
		return v * coeff;
	}
}