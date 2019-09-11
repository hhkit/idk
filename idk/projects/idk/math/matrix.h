#pragma once
#include <utility>

#include "Vector.h"

namespace idk
{
	struct col_major {};

	// matrix is column order
	template<typename T, unsigned R, unsigned C>
	struct tmat
	{
		using column_t = tvec<T, R>;
		column_t intern[C]{};

		// constructors
		tmat();
		
		// column-major constructor
		template<typename ... U,
			typename = std::enable_if_t<(std::is_same_v<U, T> && ...)>
		>
		explicit tmat(const tvec<U, R>& ... vectors);

		// row-major constructor
		template<typename ... U,
			typename = std::enable_if_t<(sizeof...(U) == R * C) && ((std::is_arithmetic_v<U>) && ...)>
		>
		explicit tmat(U ... values);

		// upcast constructor
		explicit tmat(const tmat<T, R - 1, C - 1>&);

		// downcast constructor
		explicit tmat(const tmat<T, R + 1, C + 1>&);

		// common functions
		T                determinant() const;
		tmat<T, C, R>  transpose() const;
		tmat<T, C, R>  inverse() const;

		// utility
		tmat<T, C-1, R-1> cofactor(unsigned r, unsigned c) const;
		//matrix&             gauss_jordan();

		// accessors
		column_t*       begin();
		const column_t* begin() const;
		column_t*       end();
		const column_t* end() const;
		T*              data();
		const T*        data() const;
		column_t&       operator[](size_t index);
		const column_t& operator[](size_t index) const;

		// operator overloads
		tmat&         operator+=(const tmat& rhs);
		tmat          operator+(const tmat& rhs) const;
		tmat&         operator-=(const tmat& rhs);
		tmat          operator-(const tmat& rhs) const;
		tmat&         operator*=(const T& val);
		tmat          operator*(const T& val) const;
		tmat&         operator/=(const T& val);
		tmat          operator/(const T& val) const;
		bool            operator==(const tmat&) const;
		bool            operator!=(const tmat&) const;
	};

	template<typename T, unsigned R, unsigned C>
	tmat<T,R,C> operator*(const T& coeff, const tmat<T, R, C>&);

	template<typename T, unsigned R, unsigned C>
	tvec<T, R> operator*(const tmat<T, R, C>& lhs, const tvec<T, C>& rhs);

	template<typename T, unsigned O, unsigned M, unsigned I>
	tmat<T, O, I> operator*(const tmat<T, O, M>& lhs, const tmat<T, M, I>& rhs);

	// explicit instantiations to save on compile time
	extern template struct tmat<float, 3, 3>;
	extern template struct tmat<float, 4, 4>;
	extern template tvec<float, 3>    operator*(const tmat<float, 3, 3>&, const tvec<float, 3>&);
	extern template tvec<float, 4>    operator*(const tmat<float, 4, 4>&, const tvec<float, 4>&);
	extern template tmat<float, 3, 3> operator*(const tmat<float, 3, 3> & lhs, const tmat<float, 3, 3> & rhs);
	extern template tmat<float, 4, 4> operator*(const tmat<float, 4, 4> & lhs, const tmat<float, 4, 4> & rhs);
}

#include "Matrix.inl"