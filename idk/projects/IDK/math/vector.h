#pragma once
#include <utility>

#include "Vector_detail.h"
#include "Vector_Dim.h"

namespace idk
{
	template<typename T, unsigned D>
	struct vector : detail::vector_base<T, D>
	{
		using Base = typename detail::vector_base<T, D>;
		using Base::Base;
		using Base::begin;
		using Base::end;

		template<typename ... Args,
			typename = std::enable_if_t<(detail::Dim_v<Args> + ...) == D>,
			typename = std::enable_if_t<(!std::is_same_v<vector, Args> && ...)>
		>
		constexpr vector(Args ...);

		// vector functions
		T        magnitude_sq() const;
		T        magnitude() const;
		T        distance_sq(const vector&) const;
		T        distance(const vector&) const;

		T        dot(const vector&) const;
		vector&  normalize();
		vector   get_normalized() const;
		T*       data();
		const T* data() const;

		// member functions
		vector&  operator+=(const vector&);	
		vector   operator+(const vector&) const;

		vector   operator-() const;
		vector&  operator-=(const vector&);
		vector   operator-(const vector&) const;

		vector&  operator*=(const vector&);
		vector   operator*(const vector&) const;
		vector&  operator*=(const T&);
		vector   operator*(const T&) const;

		vector&  operator/=(const vector&);
		vector   operator/(const vector&) const;
		vector&  operator/=(const T&);
		vector   operator/(const T&) const;

		bool     operator==(const vector&) const;
		bool     operator!=(const vector&) const;

		// accessors
		T&       operator[](unsigned i);
		const T& operator[](unsigned i) const;

	};

	extern template struct vector<float, 2>;
	extern template struct vector<float, 3>;
	extern template struct vector<float, 4>;
}

#include "Vector.inl"