#pragma once
#include <utility>
#include <array>
#include <cassert>

#include "Vector.h"
#include "Matrix.h"

namespace idk::math
{
	namespace detail
	{
		template<typename T, unsigned R, unsigned C, size_t ... Indexes>
		vector<T, R> MatrixVectorMult(const matrix<T, R, C>& lhs, const vector<T, C>& rhs, std::index_sequence<Indexes...>)
		{
			return ((lhs[Indexes] * rhs[Indexes]) + ...);
		}

		template<typename T, unsigned O, unsigned M, unsigned I, size_t ... Indexes>
		matrix<T, O, I> MatrixMatrixMult(const matrix<T, O, M>& lhs, const matrix<T, M, I>& rhs, std::index_sequence<Indexes...>)
		{
			return matrix<T, O, I>{ (lhs * rhs[Indexes]) ...};
		}

		template<size_t TransposeMe, typename T, unsigned R, unsigned C, size_t ... RowIndexes>
		auto MatrixTransposeRow(const matrix<T, R, C>& transposeme,
			std::index_sequence<RowIndexes...>)
		{
			return vector<T, C>{
				(transposeme[TransposeMe][RowIndexes]) ...
			};
		}

		template<typename T, unsigned R, unsigned C, size_t ... RowIndexes>
		matrix<T, C, R> MatrixTranspose(const matrix<T, R, C>& transposeme, 
			std::index_sequence<RowIndexes...>)
		{
			return matrix<T, C, R> {
					(MatrixTransposeRow<RowIndexes>(transposeme, std::make_index_sequence<C>{})) ...
				};
		}

		template<unsigned Col, typename T, unsigned R, unsigned C, 
			unsigned ... RowIndexes>
		vector<T, C> VectorOnRow(const std::array<T, R* C>& values, std::index_sequence<RowIndexes...>)
		{
			return vector<T, C>{
				(values[R * RowIndexes + Col]) ...
			};
		}

		template<typename T, unsigned R, unsigned C, 
			size_t ... ColIndexes>
		matrix<T, R, C> MatrixFromRowMajor(const std::array<T, R*C> &values, std::index_sequence<ColIndexes...>)
		{
			return matrix<T, R, C>{
				(VectorOnRow<ColIndexes, T, R, C>(values, std::make_index_sequence<R>{})) ...
			};
		}
	}

	template<typename T, unsigned R, unsigned C>
	matrix<T, R, C>::matrix()
		: intern{}
	{
	}
	
	template<typename T, unsigned R, unsigned C>
	template<typename ...U, typename>
	matrix<T, R, C>::matrix(const vector<U, R>& ...vectors)
		: intern{vectors...}
	{
	}

	template<typename T, unsigned R, unsigned C>
	template<typename ...U, typename>
	matrix<T, R, C>::matrix(U ... values)
		: matrix{ detail::MatrixFromRowMajor<T,R,C>(std::array<T, R*C>{values...}, std::make_index_sequence<C>{}) }
	{
	}

	template<typename T, unsigned R, unsigned C>
	T matrix<T, R, C>::determinant() const
	{
		static_assert(R == C, "determinant can only be called on square matrices");
		//static_assert(R < 4,  "we haven't developed determinants beyond this dimension");

		auto& m = *this;
		if constexpr (R == 1)
			return m[0][0];
		else
		if constexpr (R == 2)
			return m[0][0] * m[1][1] - m[1][0] * m[0][1];
		else
		if constexpr (R == 3)
		{
			auto& a = m[0][0]; auto& b = m[1][0]; auto& c = m[2][0];
			auto& d = m[0][1]; auto& e = m[1][1]; auto& f = m[2][1];
			auto& g = m[0][2]; auto& h = m[1][2]; auto& i = m[2][2];

			// det of 3x3 = aei + bfg + cdh - ceg - bdi - afh;
			return a * (e * i - f * h) + b * (f * g - d * i) + c * (d * h - e * g);
		}
		else
		{
			(m);
			return 0;
		}
	}

	template<typename T, unsigned R, unsigned C>
	inline matrix<T, C, R> matrix<T, R, C>::transpose() const
	{
		return detail::MatrixTranspose(*this, std::make_index_sequence<R>{});
	}

	template<typename T, unsigned R, unsigned C>
	inline matrix<T, C, R> matrix<T, R, C>::inverse() const
	{
		static_assert(R == C, "inverse is only callable on square matrices");

		if constexpr (R == 1)
			return matrix{ 1.f } / determinant();
		else
		if constexpr (R == 2)
			return matrix{
				 intern[1][1],	-intern[1][0],
				-intern[0][1],	 intern[0][0]
			};
		else
		if constexpr(R == 3)
		{
			auto& m = *this;
			auto& a = m[0][0]; auto& b = m[1][0]; auto& c = m[2][0];
			auto& d = m[0][1]; auto& e = m[1][1]; auto& f = m[2][1];
			auto& g = m[0][2]; auto& h = m[1][2]; auto& i = m[2][2];

			return matrix{
				  (e * i - f * h), - (b * i - c * h),   (b * f - c * e),
				- (d * i - f * g),   (a * i - c * g), - (a * f - c * d),
				  (d * h - e * g), - (a * h - b * g),   (a * e - b * d)
			} / determinant();
		}
		else
		return matrix<T, C, R>();
	}

	template<typename T, unsigned R, unsigned C>
	matrix<T, C - 1, R - 1> matrix<T, R, C>::cofactor(unsigned r, unsigned c) const
	{
		auto retval = matrix<T, C-1, R-1>{};

		for (unsigned i_track = 0, i_write = 0; i_track < R; ++i_track)
		{
			if (i_track == r)
				continue;

			for (unsigned j_track = 0, j_write = 0; j_track < C; ++j_track)
			{
				if (j_track == c)
					continue;

				retval[j_write++][i_write] = (*this)[j_track][i_track];
			}

			++i_write;
		}

		return retval;
	}

	template<typename T, unsigned R, unsigned C>
	typename matrix<T, R, C>::column_t* matrix<T, R, C>::begin()
	{
		return std::begin(intern);
	}

	template<typename T, unsigned R, unsigned C>
	const typename matrix<T, R, C>::column_t* matrix<T, R, C>::begin() const
	{
		return std::begin(intern);
	}

	template<typename T, unsigned R, unsigned C>
	typename matrix<T, R, C>::column_t* matrix<T, R, C>::end()
	{
		return std::end(intern);
	}

	template<typename T, unsigned R, unsigned C>
	const typename matrix<T, R, C>::column_t* matrix<T, R, C>::end() const
	{
		return std::end(intern);
	}

	template<typename T, unsigned R, unsigned C>
	T* matrix<T, R, C>::data()
	{
		return intern->data();
	}

	template<typename T, unsigned R, unsigned C>
	const T* matrix<T, R, C>::data() const
	{
		return intern->data();
	}

	template<typename T, unsigned R, unsigned C>
	typename matrix<T, R, C>::column_t& matrix<T,R,C>::operator[](unsigned index)
	{
		return intern[index];
	}

	template<typename T, unsigned R, unsigned C>
	const typename matrix<T, R, C>::column_t& matrix<T, R, C>::operator[](unsigned index) const
	{
		return intern[index];
	}

	template<typename T, unsigned R, unsigned C>
	matrix<T, R, C>& matrix<T, R, C>::operator+=(const matrix& rhs)
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			*ltr++ += *rtr++;

		return *this;
	}

	template<typename T, unsigned R, unsigned C>
	matrix<T, R, C> matrix<T, R, C>::operator+(const matrix& rhs) const
	{
		auto copy = *this;
		return copy += rhs;
	}

	template<typename T, unsigned R, unsigned C>
	matrix<T, R, C>& matrix<T, R, C>::operator-=(const matrix& rhs)
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			* ltr++ -= *rtr++;

		return *this;
	}

	template<typename T, unsigned R, unsigned C>
	matrix<T, R, C> matrix<T, R, C>::operator-(const matrix& rhs) const
	{
		auto copy = *this;
		return copy -= rhs;
	}

	template<typename T, unsigned R, unsigned C>
	matrix<T, R, C>& matrix<T, R, C>::operator*=(const T& val)
	{
		for (auto& elem : *this)
			elem *= val;

		return *this;
	}

	template<typename T, unsigned R, unsigned C>
	matrix<T, R, C> matrix<T, R, C>::operator*(const T& val) const
	{
		auto copy = *this;
		return copy *= val;
	}


	template<typename T, unsigned R, unsigned C>
	matrix<T, R, C>& matrix<T, R, C>::operator/=(const T& val)
	{
		for (auto& elem : *this)
			elem /= val;

		return *this;
	}

	template<typename T, unsigned R, unsigned C>
	matrix<T, R, C> matrix<T, R, C>::operator/(const T& val) const
	{
		auto copy = *this;
		return copy /= val;
	}

	template<typename T, unsigned R, unsigned C>
	bool matrix<T, R, C>::operator==(const matrix& rhs) const
	{
		auto ltr = this->begin();
		auto rtr = rhs.begin();
		auto etr = this->end();

		while (ltr != etr)
			if (*ltr++ != *rtr++)
				return false;

		return true;
	}

	template<typename T, unsigned R, unsigned C>
	bool matrix<T, R, C>::operator!=(const matrix& rhs) const
	{
		return !operator==(rhs);
	}

	template<typename T, unsigned R, unsigned C>
	vector<T, R> operator*(const matrix<T, R, C>& lhs, const vector<T, C>& rhs)
	{
		return detail::MatrixVectorMult(lhs, rhs, std::make_index_sequence<C>{});
	}

	template<typename T, unsigned O, unsigned M, unsigned I>
	matrix<T, O, I> operator*(const matrix<T, O, M>& lhs, const matrix<T, M, I>& rhs)
	{
		return detail::MatrixMatrixMult(lhs, rhs, std::make_index_sequence<I>{});
	}
}