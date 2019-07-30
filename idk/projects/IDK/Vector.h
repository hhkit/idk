#pragma once
#include <utility>

#include "Vector_detail.h"
#include "Vector_Dim.h"

namespace idk
{
	template<typename T, unsigned D>
	struct Vector : detail::Vector_base<T, D>
	{
		using Base = typename detail::Vector_base<T, D>;
		using Base::Base;

		// why do i have to bring these into this space??
		using Base::begin;
		using Base::end;

		template<typename ... Args,
			typename = std::enable_if_t<(detail::Dim_v<Args> + ...) == 4>,
			typename = std::enable_if_t<!(std::is_same_v<Vector, Args> && ...)>
		>
		Vector(Args ...);

		// vector functions
		T        magnitude_sq() const;
		T        magnitude() const;
		T        distance_sq(const Vector&) const;
		T        distance(const Vector&) const;

		T        dot(const Vector&) const;
		Vector&  normalize();
		Vector   get_normalized() const;
		T*       data();
		const T* data() const;

		// member functions
		Vector&  operator+=(const Vector&);	
		Vector   operator+(const Vector&) const;

		Vector&  operator-=(const Vector&);
		Vector   operator-() const;
		Vector   operator-(const Vector&) const;

		template<typename U>
		Vector&  operator*= (const U&);
		template<typename U>
		Vector   operator *  (const U&) const;

		bool     operator==(const Vector&) const;
		bool     operator!=(const Vector&) const;

		// accessors
		T&       operator[](unsigned i);
		const T& operator[](unsigned i) const;
	};

	extern template struct Vector<float, 2>;
	extern template struct Vector<float, 3>;
	extern template struct Vector<float, 4>;
}

#include "Vector.inl"