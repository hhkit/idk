#include <cmath>
#include <tuple>

#include "Vector_detail.h"
#pragma once

namespace idk::detail
{
	template<typename T, unsigned D>
	T* Vector_base<T, D>::begin()
	{
		return std::begin(arr);
	}

	template<typename T, unsigned D>
	const T* Vector_base<T, D>::begin() const
	{
		return std::begin(arr);
	}

	template<typename T, unsigned D>
	T* Vector_base<T, D>::end()
	{
		return std::end(arr);
	}

	template<typename T, unsigned D>
	const T* Vector_base<T, D>::end() const
	{
		return std::end(arr);
	}

	template<typename T>
	T* Vector_base<T, 1>::begin()
	{
		return &x;
	}

	template<typename T>
	const T* Vector_base<T, 1>::begin() const
	{
		return &x;
	}

	template<typename T>
	T* Vector_base<T, 1>::end()
	{
		return &x + 1;
	}

	template<typename T>
	const T* Vector_base<T, 1>::end() const
	{
		return &x + 1;
	}

	template<typename T>
	T* Vector_base<T, 2>::begin()
	{
		return &x;
	}

	template<typename T>
	const T* Vector_base<T, 2>::begin() const
	{
		return &x;
	}

	template<typename T>
	T* Vector_base<T, 2>::end()
	{
		return &x + 2;
	}

	template<typename T>
	const T* Vector_base<T, 2>::end() const
	{
		return &x + 2;
	}

	template<typename T>
	T* Vector_base<T, 3>::begin()
	{
		return &x;
	}

	template<typename T>
	const T* Vector_base<T, 3>::begin() const
	{
		return &x;
	}

	template<typename T>
	T* Vector_base<T, 3>::end()
	{
		return &x + 3;
	}

	template<typename T>
	const T* Vector_base<T, 3>::end() const
	{
		return &x + 3;
	}

	template<typename T>
	T* Vector_base<T, 4>::begin()
	{
		return &x;
	}

	template<typename T>
	const T* Vector_base<T, 4>::begin() const
	{
		return &x;
	}

	template<typename T>
	T* Vector_base<T, 4>::end()
	{
		return &x + 4;
	}

	template<typename T>
	const T* Vector_base<T, 4>::end() const
	{
		return &x + 4;
	}


	template<typename T>
	Vector_base<T, 1>::Vector_base()
		: x{}
	{
	}

	template<typename T>
	Vector_base<T, 1>::Vector_base(T x)
		: x{ x }
	{
	}

	template<typename T>
	Vector_base<T, 2>::Vector_base()
		: x{}, y{}
	{
	}


	template<typename T>
	Vector_base<T, 2>::Vector_base(T x, T y)
		: x{ x }, y{ y }
	{
	}

	template<typename T>
	T Vector_base<T, 2>::angle() const
	{
		if constexpr (std::is_same_v<T, float>)
			return atan2f(y, x);
		else
			return atan2(y, x);
	}

	template<typename T>
	T Vector_base<T, 2>::cross(const Vector_base& rhs) const
	{
		return x * rhs.y - y * rhs.x;
	}

	template<typename T>
	Vector_base<T, 3>::Vector_base()
		: x{}, y{}, z{}
	{
	}


	template<typename T>
	Vector_base<T, 3>::Vector_base(T x, T y, T z)
		: x{ x }, y{ y }, z{ z }
	{
	}

	template<typename T>
	Vector<T, 3> Vector_base<T, 3>::cross(const Vector_base & rhs) const
	{
		return Vector<T, 3>
		{
			y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x
		};
	}

	template<typename T>
	Vector_base<T, 4>::Vector_base()
		: x{}, y{}, z{}, w{}
	{
	}


	template<typename T>
	Vector_base<T, 4>::Vector_base(T x, T y, T z, T w)
		: x{ x }, y{ y }, z{ z }, w{ w }
	{
	}

	template<typename T, unsigned D, unsigned ... Indexes>
	auto VectorToTuple(const Vector<T, D>& vec, std::integer_sequence<size_t, Indexes...>)
	{
		return std::make_tuple(vec[Indexes]...);
	}

	template<typename T>
	auto VectorsToTuple()
	{
		return std::tuple<>{};
	}

	template<typename T, unsigned FrontD, typename ... Tail>
	auto VectorsToTuple(const Vector<T, FrontD>& front_vec, const Tail& ... tail)
	{
		return std::tuple_cat(
			VectorToTuple(front_vec, std::make_index_sequence<FrontD>{}),
			VectorsToTuple<T>(tail...)
		);
	}

	template<typename T, typename U, typename ... Tail>
	auto VectorsToTuple(const U& front_vec, const Tail& ... tail)
	{
		return std::tuple_cat(
			VectorToTuple(Vector<T, 1>{front_vec}, std::make_index_sequence<1>{}),
			VectorsToTuple<T>(tail...)
		);
	}

	template <typename T, typename Tuple, unsigned ... Indexes>
	auto TupleToVector(const Tuple& tup, std::index_sequence<Indexes...>)
	{
		return Vector<T, sizeof...(Indexes)>{std::get<Indexes>(tup)...};
	}

	template<typename T, typename ... Args>
	auto VectorConcat(const Args& ... vecs)
	{
		auto arg_tup = detail::VectorsToTuple<T>(vecs...);
		auto index = std::make_index_sequence <std::tuple_size_v<decltype(arg_tup)>>{};
		return detail::TupleToVector<float>(arg_tup, index);
	}
}