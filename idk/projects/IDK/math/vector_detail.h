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
			T data[D];
			constexpr vector_base();

			// iterator
			T* begin();
			T* end();
			const T* begin() const;
			const T* end() const;
		};

		template<typename T>
		struct vector_base<T, 1>
		{
			union
			{
				T x;
				swizzle<vector, T, 0, 0> xx;
				swizzle<vector, T, 0, 0, 0> xxx;
				swizzle<vector, T, 0, 0, 0, 0> xxxx;
			};
			constexpr vector_base();
			constexpr explicit vector_base(T x);

			// iteration
			T* begin();
			T* end();
			const T* begin() const;
			const T* end() const;
		};

		template<typename T>
		struct vector_base<T, 2>
		{
			union
			{
				struct { T x; T y; };
				#include "swizzles/swizzle2"
			};
			constexpr vector_base();
			constexpr vector_base(T x, T y);

			radian<T> angle() const;
			T cross(const vector_base&) const;

			// iteration
			T* begin();
			T* end();
			const T* begin() const;
			const T* end() const;
		};

		template<typename T>
		struct vector_base<T, 3>
		{
			union
			{
				struct { T x; T y; T z; };
				#include "swizzles/swizzle3"
			};
			constexpr vector_base();
			constexpr vector_base(T x, T y, T z);

			vector<T, 3> cross(const vector_base&) const;

			// iteration
			T* begin();
			T* end();
			const T* begin() const;
			const T* end() const;
		};

		template<typename T>
		struct vector_base<T, 4>
		{
			union
			{
				struct { T x; T y; T z; T w; };
				#include "swizzles/swizzle4"
			};

			constexpr vector_base();
			constexpr vector_base(T x, T y, T z, T w);

			// iteration
			T* begin();
			T* end();
			const T* begin() const;
			const T* end() const;
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
	}
}

#include "Vector_detail.inl"