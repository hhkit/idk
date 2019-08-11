#pragma once
#include "angle.h"
#include "vector_swizzle.h"

#pragma warning(disable:4201)


namespace idk::math
{
	template<typename T, unsigned D> struct vector;

	namespace detail
	{
		template<typename T, unsigned D>
		struct vector_base
		{
			T values[D];
			constexpr vector_base() : values{} {}
		};

		template<typename T>
		struct vector_base<T, 1>
		{
			union
			{
				T values[1];
				swizzle<vector, T, 0> x;
				swizzle<vector, T, 0, 0> xx;
				swizzle<vector, T, 0, 0, 0> xxx;
				swizzle<vector, T, 0, 0, 0, 0> xxxx;
			};
			constexpr vector_base() : values{} {}
			constexpr explicit vector_base(T x) : values{ x } {}
		};

		template<typename T>
		struct vector_base<T, 2>
		{
			union
			{
				T values[2];
				struct { T x; T y; };
				#include "swizzles/swizzle2"
			};
			constexpr vector_base() : values{} {};
			constexpr vector_base(T x, T y) : values{ x, y } {};

			radian<T> angle() const
			{
				return atan(y, x);
			}
			T cross(const vector_base& rhs) const
			{
				return x * rhs.y - y * rhs.x;
			}
		};

		template<typename T>
		struct vector_base<T, 3>
		{
			union
			{
				T values[3];
				struct { T x; T y; T z; };
				#include "swizzles/swizzle3"
			};
			constexpr vector_base() : values{} {}
			constexpr vector_base(T x, T y, T z) : values{ x, y, z } {}

			vector<T, 3> cross(const vector_base& rhs) const
			{
				return vector<T, 3>
				{
					y* rhs.z - z * rhs.y,
						z* rhs.x - x * rhs.z,
						x* rhs.y - y * rhs.x
				};
			}
		};

		template<typename T>
		struct vector_base<T, 4>
		{
			union
			{
				T values[4];
				struct { T x; T y; T z; T w; };
				#include "swizzles/swizzle4"
			};

			constexpr vector_base() : values{} {}
			constexpr vector_base(T x, T y, T z, T w) : values{ x, y, z, w } {}
		};

		template<typename T, unsigned D, unsigned ... Indexes>
		constexpr auto VectorToTuple(const vector<T, D>& vec, std::integer_sequence<size_t, Indexes...>);

		template<typename T>
		constexpr auto VectorsToTuple();

		template<typename T, unsigned FrontD, typename ... Tail>
		constexpr auto VectorsToTuple(const vector<T, FrontD>& front_vec, const Tail& ... tail);

		template<typename T, typename ... Tail>
		constexpr auto VectorsToTuple(const T& front_vec, const Tail& ... tail);

		template <typename T, typename Tuple, unsigned ... Indexes>
		constexpr auto TupleToVector(const Tuple& tup, std::index_sequence<Indexes...>);

		template<typename T, typename ... Args>
		constexpr auto VectorConcat(const Args& ... vecs);
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
			return vector<T, sizeof...(Indexes)>{static_cast<T>(std::get<Indexes>(tup))...};
		}

		template<typename T, typename ... Args>
		constexpr auto VectorConcat(const Args& ... vecs)
		{
			auto arg_tup = detail::VectorsToTuple(vecs...);
			auto index = std::make_index_sequence <std::tuple_size_v<decltype(arg_tup)>>{};
			return detail::TupleToVector<T>(arg_tup, index);
		}
	}
}