#pragma once
#include <utility>

#include "Vector.h"

namespace idk::math
{
	struct col_major {};

	// matrix is column order
	template<typename T, unsigned R, unsigned C>
	struct matrix
	{
		using column_t = vector<T, R>;
		column_t intern[C]{};

		// constructors
		matrix();
		
		// column-major constructor
		template<typename ... U,
			typename = std::enable_if_t<(std::is_same_v<U, T> && ...)>
		>
		explicit matrix(const vector<U, R>& ... vectors);

		// row-major constructor
		template<typename ... U,
			typename = std::enable_if_t<(sizeof...(U) == R * C) && ((std::is_arithmetic_v<U>) && ...)>
		>
		explicit matrix(U ... values);

		// upcast constructor
		explicit matrix(const matrix<T, R - 1, C - 1>&);

		// common functions
		T                determinant() const;
		matrix<T, C, R>  transpose() const;
		matrix<T, C, R>  inverse() const;

		// utility
		matrix<T, C-1, R-1> cofactor(unsigned r, unsigned c) const;
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
		matrix&         operator+=(const matrix& rhs);
		matrix          operator+(const matrix& rhs) const;
		matrix&         operator-=(const matrix& rhs);
		matrix          operator-(const matrix& rhs) const;
		matrix&         operator*=(const T& val);
		matrix          operator*(const T& val) const;
		matrix&         operator/=(const T& val);
		matrix          operator/(const T& val) const;
		bool            operator==(const matrix&) const;
		bool            operator!=(const matrix&) const;
	};

	template<typename T, unsigned R, unsigned C>
	matrix<T,R,C> operator*(const T& coeff, const matrix<T, R, C>&);

	template<typename T, unsigned R, unsigned C>
	vector<T, R> operator*(const matrix<T, R, C>& lhs, const vector<T, C>& rhs);

	template<typename T, unsigned O, unsigned M, unsigned I>
	matrix<T, O, I> operator*(const matrix<T, O, M>& lhs, const matrix<T, M, I>& rhs);

	// explicit instantiations to save on compile time
	extern template struct matrix<float, 3, 3>;
	extern template struct matrix<float, 4, 4>;
	extern template vector<float, 3>    operator*(const matrix<float, 3, 3>&, const vector<float, 3>&);
	extern template vector<float, 4>    operator*(const matrix<float, 4, 4>&, const vector<float, 4>&);
	extern template matrix<float, 3, 3> operator*(const matrix<float, 3, 3> & lhs, const matrix<float, 3, 3> & rhs);
	extern template matrix<float, 4, 4> operator*(const matrix<float, 4, 4> & lhs, const matrix<float, 4, 4> & rhs);
}

#include "Matrix.inl"