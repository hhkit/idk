#include <cmath>
#include <tuple>

#include "vector_detail.h"
#include "angle.h"
#pragma once

namespace idk::math::detail
{
	template<typename T, unsigned D>
	T* vector_base<T, D>::begin()
	{
		return std::begin(data);
	}

	template<typename T, unsigned D>
	const T* vector_base<T, D>::begin() const
	{
		return std::begin(data);
	}

	template<typename T, unsigned D>
	T* vector_base<T, D>::end()
	{
		return std::end(data);
	}

	template<typename T, unsigned D>
	const T* vector_base<T, D>::end() const
	{
		return std::end(data);
	}

	template<typename T>
	T* vector_base<T, 1>::begin()
	{
		return &x;
	}

	template<typename T>
	const T* vector_base<T, 1>::begin() const
	{
		return &x;
	}

	template<typename T>
	T* vector_base<T, 1>::end()
	{
		return &x + 1;
	}

	template<typename T>
	const T* vector_base<T, 1>::end() const
	{
		return &x + 1;
	}

	template<typename T>
	T* vector_base<T, 2>::begin()
	{
		return &x;
	}

	template<typename T>
	const T* vector_base<T, 2>::begin() const
	{
		return &x;
	}

	template<typename T>
	T* vector_base<T, 2>::end()
	{
		return &x + 2;
	}

	template<typename T>
	const T* vector_base<T, 2>::end() const
	{
		return &x + 2;
	}

	template<typename T>
	T* vector_base<T, 3>::begin()
	{
		return &x;
	}

	template<typename T>
	const T* vector_base<T, 3>::begin() const
	{
		return &x;
	}

	template<typename T>
	T* vector_base<T, 3>::end()
	{
		return &x + 3;
	}

	template<typename T>
	const T* vector_base<T, 3>::end() const
	{
		return &x + 3;
	}

	template<typename T>
	T* vector_base<T, 4>::begin()
	{
		return &x;
	}

	template<typename T>
	const T* vector_base<T, 4>::begin() const
	{
		return &x;
	}

	template<typename T>
	T* vector_base<T, 4>::end()
	{
		return &x + 4;
	}

	template<typename T>
	const T* vector_base<T, 4>::end() const
	{
		return &x + 4;
	}


	template<typename T>
	constexpr vector_base<T, 1>::vector_base()
		: x{}
	{
	}

	template<typename T>
	constexpr vector_base<T, 1>::vector_base(T x)
		: x{ x }
	{
	}

	template<typename T>
	constexpr vector_base<T, 2>::vector_base()
		: x{}, y{}
	{
	}


	template<typename T>
	constexpr vector_base<T, 2>::vector_base(T x, T y)
		: x{ x }, y{ y }
	{
	}

	template<typename T>
	radian<T> vector_base<T, 2>::angle() const
	{
		atan(y, x);
	}

	template<typename T>
	T vector_base<T, 2>::cross(const vector_base& rhs) const
	{
		return x * rhs.y - y * rhs.x;
	}

	template<typename T>
	constexpr vector_base<T, 3>::vector_base()
		: x{}, y{}, z{}
	{
	}


	template<typename T>
	constexpr vector_base<T, 3>::vector_base(T x, T y, T z)
		: x{ x }, y{ y }, z{ z }
	{
	}

	template<typename T>
	vector<T, 3> vector_base<T, 3>::cross(const vector_base & rhs) const
	{
		return vector<T, 3>
		{
			y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x
		};
	}

	template<typename T>
	constexpr vector_base<T, 4>::vector_base()
		: x{}, y{}, z{}, w{}
	{
	}


	template<typename T>
	constexpr vector_base<T, 4>::vector_base(T x, T y, T z, T w)
		: x{ x }, y{ y }, z{ z }, w{ w }
	{
	}

	template<typename T, unsigned D, unsigned ... Indexes>
	constexpr auto VectorToTuple(const vector<T, D>& vec, std::integer_sequence<size_t, Indexes...>)
	{
		return std::forward_as_tuple(vec[Indexes]...);
	}

	template<typename T>
	constexpr auto VectorsToTuple()
	{
		return std::tuple<>{};
	}

	template<typename T, unsigned FrontD, typename ... Tail>
	constexpr auto VectorsToTuple(const vector<T, FrontD>& front_vec, const Tail& ... tail)
	{
		return std::tuple_cat(
			VectorToTuple<T>(front_vec, std::make_index_sequence<FrontD>{}),
			VectorsToTuple<T>(tail...)
		);
	}

	template<typename T, typename ... Tail>
	constexpr auto VectorsToTuple(const T& front_vec, const Tail& ... tail)
	{
		return std::tuple_cat(
			std::tuple<T>(front_vec),
			VectorsToTuple<T>(tail...)
		);
	}

	template <typename T, typename Tuple, unsigned ... Indexes>
	constexpr auto TupleToVector(const Tuple& tup, std::index_sequence<Indexes...>)
	{
		return vector<T, sizeof...(Indexes)>{std::get<Indexes>(tup)...};
	}

	template<typename T, typename ... Args>
	constexpr auto VectorConcat(const Args& ... vecs)
	{
		auto arg_tup = detail::VectorsToTuple<T>(vecs...);
		auto index = std::make_index_sequence <std::tuple_size_v<decltype(arg_tup)>>{};
		return detail::TupleToVector<float>(arg_tup, index);
	}
}