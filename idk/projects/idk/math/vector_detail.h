#pragma once
#include <machine.h>
#include <xmmintrin.h>

#include "angle.h"
#include "vector_swizzle.h"
#include "vector_dim.h"

#pragma warning(disable:4324) // padding
#pragma warning(disable:4201) // unnamed struct

namespace idk
{
	template<typename T, unsigned D> struct tvec;

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
				swizzle<tvec, T, 0> x;
				swizzle<tvec, T, 0, 0> xx;
				swizzle<tvec, T, 0, 0, 0> xxx;
				swizzle<tvec, T, 0, 0, 0, 0> xxxx;
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

			trad<T> angle() const
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

			tvec<T, 3> cross(const vector_base& rhs) const
			{
				return tvec<T, 3>
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

		template<>
		struct alignas(16) vector_base<float, 4>
		{
			using T = float;
			union
			{
				T values[4];
				struct { T x; T y; T z; T w; };
				__m128 sse;
				#include "swizzles/swizzle4"
			};

			constexpr vector_base() : values{} {}
			constexpr explicit vector_base(__m128 packed) : sse{ packed } {}
			constexpr vector_base(T x, T y, T z, T w) : values{ x, y, z, w } {}
		};

		template<typename T>
		struct is_vector : std::false_type {};

		template<typename T, unsigned D>
		struct is_vector<tvec<T, D>> : std::true_type {};


		template<typename T, typename ... Args>
		constexpr auto VectorConcat(const Args& ... vecs);
		template<typename T, unsigned D, unsigned ... Indexes>
		constexpr auto VectorToTuple(const tvec<T, D>& vec, std::index_sequence<Indexes...>)
		{
			return std::make_tuple(vec[Indexes]...);
		}

		template<typename T>
		constexpr inline auto VectorToTuple(const T& val, std::index_sequence<0>)
		{
			return std::tuple<T>{val};
		}

		template<typename ... Args, unsigned ... Dims>
		constexpr auto VectorsToTuple(std::index_sequence<Dims...>, const Args& ... vecs)
		{
			return std::tuple_cat(
				VectorToTuple(vecs, std::make_index_sequence<Dims>{})...
			);
		}


		template <typename T, typename Tuple, unsigned ... Indexes>
		constexpr auto TupleToVector(const Tuple& tup, std::index_sequence<Indexes...>)
		{
			return tvec<T, sizeof...(Indexes)>{static_cast<T>(std::get<Indexes>(tup))...};
		}

		template<typename T, typename ... Args>
		constexpr auto VectorConcat(const Args& ... vecs)
		{
			constexpr auto index_seq = std::index_sequence<detail::Dim_v<Args>...>{};
			using tuple_type = decltype(VectorsToTuple(index_seq, vecs...));
			return TupleToVector<T>(VectorsToTuple(index_seq, vecs...), std::make_index_sequence <std::tuple_size_v<tuple_type>>{});
		}

		template<>
		inline auto VectorConcat<float>(const tvec<float, 3>& vec, const int& homogenous);

		template<>
		inline auto VectorConcat<float>(const float& x, const float&y, const float& z, const int& homogenous);

		template<typename T>
		struct Abs {};
		template<unsigned...indices>
		struct Abs<std::index_sequence<indices...>>
		{
			template<typename T>
			static auto abs(const T& lhs)
			{
				return T{ std::abs(lhs[indices])... };
			}
		};
	}
}