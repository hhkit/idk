#pragma once
#include <util/range.h>
#include <util/zip.h>
#include "matrix_transforms.h"

namespace idk::math
{
	template<typename T>
	T identity()
	{
		return T();
	}
	template<typename T, unsigned D>
	matrix<T, D, D> scale(const vector<T, D>& rhs)
	{
		matrix<T, D, D> retval;
		for (auto& [row, scale, n] : zip(retval, rhs, range<D>()))
			row[n] = scale;
		return retval;
	}

	template<typename T>
	matrix<T, 3, 3> rotate(const vector<T, 3> & axis, radian<T> angle)
	{
		const auto nAxis = axis.get_normalized();
		const auto c = cos(angle.value);
		const auto s = sin(angle.value);


		return matrix<T, 3, 3>{

		};
	}

	template<typename T>
	matrix<T, 3, 3> rotate(const vector<T, 3> & axis, radian<T> angle);

	template<typename T, unsigned D>
	matrix<T, D + 1, D + 1> trans(const vector<T, D> & trans);
}