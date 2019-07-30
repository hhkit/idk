#pragma once
namespace idk
{
	template<typename T, unsigned D> struct Vector;

	namespace detail
	{
		template<typename T, unsigned D>
		struct Vector_base
		{
			T arr[D];
			Vector_base();

			// iterator
			T* begin();
			T* end();
			const T* begin() const;
			const T* end() const;
		};

		template<typename T>
		struct Vector_base<T, 1>
		{
			T x;
			Vector_base();
			explicit Vector_base(T x);

			// iteration
			T* begin();
			T* end();
			const T* begin() const;
			const T* end() const;
		};

		template<typename T>
		struct Vector_base<T, 2>
		{
			T x;
			T y;
			Vector_base();
			Vector_base(T x, T y);

			T angle() const;
			T cross(const Vector_base&) const;

			// iteration
			T* begin();
			T* end();
			const T* begin() const;
			const T* end() const;
		};

		template<typename T>
		struct Vector_base<T, 3>
		{
			T x;
			T y;
			T z;
			Vector_base();
			Vector_base(T x, T y, T z);

			Vector<T, 3> cross(const Vector_base&) const;

			// iteration
			T* begin();
			T* end();
			const T* begin() const;
			const T* end() const;
		};

		template<typename T>
		struct Vector_base<T, 4>
		{
			T x;
			T y;
			T z;
			T w;

			Vector_base();
			Vector_base(T x, T y, T z, T w);

			// iteration
			T* begin();
			T* end();
			const T* begin() const;
			const T* end() const;
		};

		template<typename T, unsigned D, unsigned ... Indexes>
		auto VectorToTuple(const Vector<T, D>& vec, std::integer_sequence<size_t, Indexes...>);

		template<typename T>
		auto VectorsToTuple();

		template<typename T, unsigned FrontD, typename ... Tail>
		auto VectorsToTuple(const Vector<T, FrontD>& front_vec, const Tail& ... tail);

		template<typename T, typename ... Tail>
		auto VectorsToTuple(const T& front_vec, const Tail& ... tail);

		template <typename T, typename Tuple, unsigned ... Indexes>
		auto TupleToVector(const Tuple& tup, std::index_sequence<Indexes...>);

		template<typename T, typename ... Args>
		auto VectorConcat(const Args& ... vecs);
	}
}

#include "Vector_detail.inl"