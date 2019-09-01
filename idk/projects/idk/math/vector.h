#pragma once
#include <utility>

#include "Vector_detail.h"
#include "Vector_Dim.h"

namespace idk
{
	template<typename T, unsigned D>
	struct tvec : detail::vector_base<T, D>
	{
		using Base = typename detail::vector_base<T, D>;
		using Base::Base;
		using Base::values;

		constexpr explicit tvec(const T& fill);

		template<typename ... Args,
			typename = std::enable_if_t<
				(detail::Dim_v<Args> + ...) == D &&
			    ((detail::Dim_v<Args> > 0) && ...) &&
			    ((!std::is_same_v<tvec, Args>) && ...)
			>
		>
		constexpr explicit tvec(const Args& ...);

		template<unsigned D2,
			typename = std::enable_if_t<(D2 > D)>
		>
		constexpr tvec(const tvec<T, D2>&);

		// vector functions
		T        length_sq() const;
		T        length() const;
		T        distance_sq(const tvec&) const;
		T        distance(const tvec&) const;

		T        dot(const tvec&) const;
		tvec     project_onto(const tvec&)const;
		tvec&  normalize();
		tvec   get_normalized() const;

		// iteration
		constexpr T*       begin() noexcept;
		constexpr T*       end() noexcept;
		constexpr const T* begin() const noexcept;
		constexpr const T* end() const noexcept;
		constexpr T*       data() noexcept;
		constexpr const T* data() const noexcept;

		// member functions
		tvec&  operator+=(const tvec&);	
		tvec   operator+(const tvec&) const;

		tvec   operator-() const;
		tvec&  operator-=(const tvec&);
		tvec   operator-(const tvec&) const;

		tvec&  operator*=(const tvec&);
		tvec   operator*(const tvec&) const;
		tvec&  operator*=(const T&);
		tvec   operator*(const T&) const;

		tvec&  operator/=(const tvec&);
		tvec   operator/(const tvec&) const;
		tvec&  operator/=(const T&);
		tvec   operator/(const T&) const;

		bool     operator==(const tvec&) const;
		bool     operator!=(const tvec&) const;

		// accessors
		constexpr T&       operator[](size_t i) noexcept;
		constexpr const T& operator[](size_t i) const noexcept;

	};

	template <typename T, unsigned D>
	tvec<T,D> operator*(const T&, const tvec<T, D>&);

	template<typename T, unsigned D>
	auto dot(const tvec<T, D>& lhs, const tvec<T, D>& rhs) 
	{
		return lhs.dot(rhs);
	}

	template<typename T, unsigned D>
	auto vabs(const tvec<T, D>& lhs)
	{
		return detail::Abs<std::make_index_sequence<D>>::abs(lhs);
	}

	extern template struct tvec<float, 2>;
	extern template struct tvec<float, 3>;
	extern template struct tvec<float, 4>;
}

#include "Vector.inl"