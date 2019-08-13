#pragma once
#include <type_traits>

namespace idk::math
{
	template<typename T, unsigned D> struct vector;

	namespace detail
	{
		template <typename T>
		struct Dim
		{
			static constexpr unsigned value = std::is_arithmetic_v<T> ? 1 : 16;
		};

		template<typename T>
		static constexpr unsigned Dim_v = Dim<T>::value;

		template <typename T, unsigned D>
		struct Dim < vector<T, D>>
		{
			static constexpr unsigned value = Dim_v<T> * D;
		};
	}
}