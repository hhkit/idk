#pragma once

template<typename T, unsigned D> struct vector;

namespace idk::math
{
	namespace detail
	{
		template <typename T>
		struct Dim;

		template<typename T>
		static constexpr unsigned Dim_v = Dim<T>::value;

		template <typename T, unsigned D>
		struct Dim < vector<T, D>>
		{
			static constexpr unsigned value = Dim_v<T> * D;
		};

		template <>
		struct Dim<float>
		{
			static constexpr unsigned value = 1;
		};

		template <>
		struct Dim<double>
		{
			static constexpr unsigned value = 1;
		};

		template <>
		struct Dim<int>
		{
			static constexpr unsigned value = 1;
		};
	}
}