#pragma once
#include <util/range.h>
#include <util/zip.h>
#include "matrix_transforms.h"

namespace idk
{
	template<typename T>
	T identity()
	{
		return T();
	}
	template<typename T, unsigned D>
	math::matrix<T, D, D> scale(const math::vector<T, D>& rhs)
	{
		math::matrix<T, D, D> retval;
		for (auto [row, scale, n] : zip(retval, rhs, range<D>()))
			row[n] = scale;
		return retval;
	}

	template<typename T>
	math::matrix<T, 3, 3> rotate(const math::vector<T, 3> & axis, math::radian<T> angle)
	{
		using ret_t = math::matrix<T, 3, 3>;
		const auto n = axis.get_normalized();
		const auto c = cos(angle);
		const auto s = sin(angle);


		return ret_t{}
		+ s * ret_t{
			 0.f, -n.z,  n.x,
			 n.z,  0.f, -n.y,
			-n.x,  n.y,  0.f
		}
		+ (1 - c) * ret_t {
			n.x * n.x,  n.x * n.y, n.x * n.z,
			n.y * n.x,  n.y * n.y, n.y * n.z,
			n.z * n.x,  n.z * n.y, n.z * n.z,
		};
	}

	template<typename T, unsigned D>
	math::matrix<T, D + 1, D + 1> translate(const math::vector<T, D> & translate_vec)
	{
		auto retval = math::matrix<T, D + 1, D + 1>();
		retval[D] = math::vector<T, D + 1>{ translate_vec, 1.f };
		return retval;
	}
}