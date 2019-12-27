#pragma once
#include <utility>
#include <cmath>

#include <math/linear.h>
#include "Vector_detail.h"
#include "Vector_Dim.h"
#undef min
#undef max

namespace idk
{
	template<typename T, unsigned D>
	struct tvec :
		linear<tvec<T, D>, T>,
		detail::vector_base<T, D>
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

		constexpr explicit tvec(T* ptr);

		template<unsigned D2,
			typename = std::enable_if_t<(D2 > D)>
		>
		constexpr tvec(const tvec<T, D2>&);

		// vector functions
		T      length_sq() const;
		T      length() const;
		T      distance_sq(const tvec&) const;
		T      distance(const tvec&) const;

		T      dot(const tvec&) const;
		tvec   project_onto(const tvec&)const;
		tvec&  normalize();
		tvec   get_normalized() const;

		// iteration
		constexpr T*       begin() noexcept;
		constexpr T*       end() noexcept;
		constexpr const T* begin() const noexcept;
		constexpr const T* end() const noexcept;
		constexpr T*       data() noexcept;
		constexpr const T* data() const noexcept;
		constexpr size_t   size() const { return D; }

		// member functions

		// accessors
		constexpr T&       operator[](size_t i) noexcept;
		constexpr const T& operator[](size_t i) const noexcept;

	};

	template<typename T, unsigned D>
	auto dot(const tvec<T, D>& lhs, const tvec<T, D>& rhs) 
	{
		return lhs.dot(rhs);
	}

	using std::abs;
	template<typename T, unsigned D>
	auto abs(const tvec<T, D>& lhs)
	{
		return detail::Abs<std::make_index_sequence<D>>::abs(lhs);
	}

	using std::min;
	using std::max;

	template<typename T, unsigned N>
	auto min(const idk::tvec<T, N>& lhs, const idk::tvec<T, N>& rhs)
	{
		idk::tvec<T, N>result{};
		for (auto i = N; i-- > 0;)
		{
			result[i] = (lhs[i] < rhs[i]) ? lhs[i] : rhs[i];
		}
		return result;
	}
	template<typename T, unsigned N>
	auto  max(const idk::tvec<T, N>& lhs, const idk::tvec<T, N>& rhs)
	{
		idk::tvec<T, N>result{};
		for (auto i = N; i-- > 0;)
		{
			result[i] = (lhs[i] > rhs[i]) ? lhs[i] : rhs[i];
		}
		return result;
	}

	extern template struct tvec<float, 2>;
	extern template struct tvec<float, 3>;
	extern template struct tvec<float, 4>;
}
