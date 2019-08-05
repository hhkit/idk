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
		for (auto [row, scale, n] : zip(retval, rhs, range<D>()))
			row[n] = scale;
		return retval;
	}

	template<typename T>
	matrix<T, 3, 3> rotate(const vector<T, 3> & axis, radian<T> angle)
	{
		const auto n = axis.get_normalized();
		const auto c = cos(angle);
		const auto s = sin(angle);


		return matrix<T, 3, 3>{} 
		+ s * matrix <T, 3, 3>{
			 0.f, -n.z,  n.x,
			 n.z,  0.f, -n.y,
			-n.x,  n.y,  0.f
		}
		+ (1 - c) * matrix<T, 3, 3>{
			n.x * n.x,  n.x * n.y, n.x * n.z,
			n.y * n.x,  n.y * n.y, n.y * n.z,
			n.z * n.x,  n.z * n.y, n.z * n.z,
		};
	}

	template<typename T, unsigned D>
	matrix<T, D + 1, D + 1> translate(const vector<T, D> & translate_vec)
	{
		auto retval = matrix<T, D + 1, D + 1>();
		retval[D] = vector<T, D + 1>{ translate_vec, 1.f };
		return retval;
	}
}