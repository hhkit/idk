#pragma once
#include <utility>

#include "Vector_detail.h"
#include "Vector_Dim.h"

namespace idk::math
{
	template<typename T, unsigned D>
	struct vector : detail::vector_base<T, D>
	{
		using Base = typename detail::vector_base<T, D>;
		using Base::Base;
		using Base::values;

		template<typename ... Args,
			typename = std::enable_if_t<
				(detail::Dim_v<Args> + ...) == D &&
			    ((detail::Dim_v<Args> > 0) && ...) &&
			    (!std::is_same_v<vector, Args> && ...)
			>
		>
		constexpr vector(const Args& ...);

		template<unsigned D2,
			typename = std::enable_if_t<(D2 > D)>
		>
		constexpr vector(const vector<T, D2>&);

		// vector functions
		T        length_sq() const;
		T        length() const;
		T        distance_sq(const vector&) const;
		T        distance(const vector&) const;

		T        dot(const vector&) const;
		vector&  normalize();
		vector   get_normalized() const;

		// iteration
		T*       begin();
		T*       end();
		const T* begin() const;
		const T* end() const;
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
		T&       operator[](size_t i);
		const T& operator[](size_t i) const;

	};

	template <typename T, unsigned D>
	vector<T,D> operator*(const T&, const vector<T, D>&);

	template<typename T, unsigned D>
	auto dot(const vector<T, D>& lhs, const vector<T, D>& rhs) 
	{
		return lhs.dot(rhs);
	}

	extern template struct vector<float, 2>;
	extern template struct vector<float, 3>;
	extern template struct vector<float, 4>;
}

#include "Vector.inl"