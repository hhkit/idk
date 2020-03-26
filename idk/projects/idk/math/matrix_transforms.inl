#pragma once
#include <ds/range.inl>
#include <ds/zip.inl>
#include "matrix_transforms.h"
#include <idk.h>

namespace idk
{
	template<typename T>
	constexpr T identity()
	{
		return T();
	}
	template<typename T, unsigned D>
	constexpr tmat<T, D+1, D+1> scale(const tvec<T, D>& rhs)
	{
		tmat<T, D+1, D+1> retval;
		for (auto [row, scale, n] : zip(retval, rhs, range<D>()))
			row[n] = scale;
		return retval;
	}

	template<typename T, unsigned D>
	constexpr tmat<T, D, D> prescale(const tmat<T, D, D>& mat, const tvec<T, D>& scale)
	{
		auto retval = mat;
		return prescale(retval, scale);
	}

	template<typename T, unsigned D>
	constexpr tmat<T, D, D>& prescale(tmat<T, D, D>& mat, const tvec<T, D>& scale)
	{
		auto scale_itr = scale.begin();
		for (auto& col : mat)
			col *= *scale_itr++;
		return mat;
	}


	template<typename T>
	tmat<T, 4, 4> rotate(const tvec<T, 3> & axis, trad<T> angle)
	{
		using ret_t = tmat<T, 4, 4>;
		using ret_3t = tmat<T, 3, 3>;
		const auto n = axis.get_normalized();
		const auto c = cos(angle);
		const auto s = sin(angle);

		const auto crosspdt = ret_3t{
			 0.f, -n.z,  n.y,
			 n.z,  0.f, -n.x,
			-n.y,  n.x,  0.f,
		};
		
		const auto skew_symmetric = ret_3t{
			n.x * n.x,  n.x * n.y, n.x * n.z,
			n.y * n.x,  n.y * n.y, n.y * n.z,
			n.z * n.x,  n.z * n.y, n.z * n.z,    
		};

		return ret_t{ c * ret_3t{}
		+s * crosspdt
		+ (1 - c) * skew_symmetric };
	}

	template<typename T, unsigned D>
	constexpr tmat<T, D + 1, D + 1> translate(const tvec<T, D> & translate_vec)
	{
		auto retval = tmat<T, D + 1, D + 1>();
		retval[D] = tvec<T, D + 1>{ translate_vec, 1.f };
		return retval;
	}

	template<typename T, unsigned D>
	constexpr tmat<T, D, D> translate(const tmat<T, D, D>& mat, const tvec<T, D - 1>& translate)
	{
		auto retval = mat;
		return translate(retval);
	}

	template<typename T, unsigned D>
	constexpr tmat<T, D, D>& translate(tmat<T, D, D>& mat, const tvec<T, D - 1>& translate)
	{
		mat[D - 1] += tvec<T, D>{translate, 0};
		return mat;
	}

	template<typename T>
	tmat<T, 4, 4> perspective(trad<T> fov, T a, T n, T f)
	{
		const auto t = tan(fov / 2);

		constexpr auto _2 = s_cast<T>(2);
		constexpr auto _1 = s_cast<T>(1);
		constexpr auto _0 = s_cast<T>(0);

		return tmat<T, 4, 4>{
			_1 / (t*a), _0  , _0                , _0                     ,
			_0        , _1/t, _0                , _0                     ,
			_0        , _0  , -(f + n) / (f - n), - (_2 * f  * n) / (f-n),
			_0        , _0  , -_1               , _0
		};
	}
	template<typename T>
	tmat<T, 4, 4> perspective(tdeg<T> fov, T aspect_ratio, T n, T f)
	{
		return perspective(trad<T>{fov}, aspect_ratio, n, f);
	}
	template<typename T>
	constexpr tmat<T, 4, 4> ortho(T l, T r, T b, T t, T n, T f)
	{
		constexpr auto _2 = s_cast<T>(2);
		constexpr auto _1 = s_cast<T>(1);
		constexpr auto _0 = s_cast<T>(0);

		using vec_t = tvec<T, 4>;
		const vec_t tvec = {-(r+l)/(r-l), -(t + b) / (t - b), -(f + n) / (f - n), _1 };

		return tmat<T, 4, 4>(
			vec_t{_2 / (r-l), _0, _0, _0},
			vec_t{_0, _2 / (t-b), _0, _0},
			vec_t{_0, _0, -_2 / (f-n), _0},
			tvec
		);
	}
	template<typename T>
	constexpr tmat<T, 4, 4> look_at(const tvec<T, 3> & eye, const tvec<T, 3> & object, const tvec<T, 3> & global_up)
	{
		const auto target = (eye - object).normalize();
		const auto right  = global_up.cross(target).normalize();
		const auto up     = target.cross(right).normalize();

		return tmat<T, 4, 4>{
			vec4{ right,  0 }, 
			vec4{ up,     0 }, 
			vec4{ target, 0 },
			vec4{ eye,    1 }
		};
	}

	template<typename T>
	tmat<T, 4, 4> orient(const tvec<T, 3> & z_prime)
	{
		const auto axis = tvec<T, 3>{0, 0, 1}.cross(z_prime);
		const auto angle = acos(z_prime.z);
		return tmat<T, 4, 4>{rotate(axis, angle)};
	}

	template<typename T, unsigned D>
	constexpr tmat<T, D, D> orthonormalize(const tmat<T, D, D>& m)
	{
		auto retval = m;
		for (auto i : range<D>())
		{
			for (auto j = 0; j < i; ++j)
			{
				const auto v = j;
				retval[i] -= retval[i].dot(retval[v]) * retval[v];
			}
			retval[i].normalize();
		}
		return retval;
	}
	template<typename T>
	constexpr tmat<T, 4, 4> invert_rotation(const tmat<T, 4, 4>& m)
	{
		auto retval = m.transpose();
		retval[0][3] = retval[1][3] = retval[2][3] = 0;
		retval[3] = vec4{-m[0].dot(m[3]), -m[1].dot(m[3]), -m[2].dot(m[3]), 1};
		return retval;
	}
}