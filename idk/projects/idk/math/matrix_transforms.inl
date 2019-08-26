#pragma once
#include <ds/range.h>
#include <ds/zip.h>
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


		return c* ret_t{}
		+ s * ret_t{
			 0.f, -n.z,  n.y,
			 n.z,  0.f, -n.x,
			-n.y,  n.x,  0.f
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

	template<typename T>
	math::matrix<T, 4, 4> perspective(math::radian<T> fov, T a, T n, T f)
	{
		auto t = tan(fov / 2);

		constexpr auto _2 = s_cast<T>(2);
		constexpr auto _1 = s_cast<T>(1);
		constexpr auto _0 = s_cast<T>(0);

		return math::matrix<T, 4, 4>{
			_1 / (t*a), _0  , _0                , _0                     ,
			_0        , _1/t, _0                , _0                     ,
			_0        , _0  , -(f + n) / (f - n), - (_2 * f  * n) / (f-n),
			_0        , _0  , -_1               , _0
		};
	}
	template<typename T>
	math::matrix<T, 4, 4> perspective(math::degree<T> fov, T aspect_ratio, T n, T f)
	{
		return perspective(math::radian<T>{fov}, aspect_ratio, n, f);
	}
	template<typename T>
	math::matrix<T, 4, 4> ortho(T l, T r, T b, T t, T n, T f)
	{
		constexpr auto _2 = s_cast<T>(2);
		constexpr auto _1 = s_cast<T>(1);
		constexpr auto _0 = s_cast<T>(0);

		using vec_t = math::vector<T, 4>;
		const vec_t tvec = {-(r+l)/(r-l), -(t + b) / (t - b), -(f + n) / (f - n), _1 };

		return math::matrix<T, 4, 4>(
			vec_t{_2 / (r-l), _0, _0, _0},
			vec_t{_0, 2 / (t-b), _0, _0},
			vec_t{_0, _0, 2 / (f-n), _0},
			tvec
		);
	}
}