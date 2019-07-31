#pragma once
namespace idk
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
			T x;
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
			T x;
			T y;
			constexpr vector_base();
			constexpr vector_base(T x, T y);

			T angle() const;
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
			T x;
			T y;
			T z;
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
			T x;
			T y;
			T z;
			T w;

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