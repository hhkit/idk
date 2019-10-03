#pragma once
#include "matrix_decomposition.h"
#include <math/matrix_transforms.h>

namespace idk
{
	template<typename T>
	inline tmat<T, 4, 4> matrix_decomposition<T>::recompose() const
	{
		return translate(this->position) * mat4 { mat3{ rotation } *idk::scale(this->scale) };
	}

	template<typename T>
	matrix_decomposition<T> decompose(const tmat<T, 4, 4> & mat)
	{
		using vec_t = tvec<T, 3>;
		auto working = mat;
		auto retval = matrix_decomposition<T>();
		retval.position = vec3{ working[3] };
		working[3] = tvec<T, 4>{ 0, 0, 0, 1 };

		retval.scale = vec_t{ working[0].length(), working[1].length(), working[2].length() };

		for (auto i : range<3>())
			working[i] /= retval.scale[i];

		retval.rotation = decompose_rotation_matrix(working).inverse();

		return retval;
	}

	template<typename T>
	quaternion<T> decompose_rotation_matrix(const tmat<T, 4, 4>& r)
	{
		quaternion<T> q;
		constexpr auto _1 = T{ 1 };
		constexpr auto _2 = T{ 2 };
		constexpr auto _0_25 = T{ 0.25 };
		constexpr auto _0_5 = T{ 0.5 };

		// from geometrictools
		auto r22 = r[2][2];
		if (r22 <= 0)  // x^2 + y^2 >= z^2 + w^2
		{
			const real dif10 = r[1][1] - r[0][0];
			const real omr22 = _1 - r22;
			if (dif10 <= 0)  // x^2 >= y^2
			{
				const real fourxsqr = omr22 - dif10;
				const real inv4x = _0_5 / sqrt(fourxsqr);
				q.x = fourxsqr * inv4x;
				q.y = (r[1][0] + r[0][1]) * inv4x;
				q.z = (r[2][0] + r[0][2]) * inv4x;
				q.w = (r[2][1] - r[1][2]) * inv4x;
			}
			else  // y^2 >= x^2
			{
				const real fourysqr = omr22 + dif10;
				const real inv4y = _0_5 / sqrt(fourysqr);
				q.x = (r[1][0] + r[0][1]) * inv4y;
				q.y = fourysqr * inv4y;
				q.z = (r[2][0] + r[0][2]) * inv4y;
				q.w = (r[2][1] - r[1][2]) * inv4y;
			}
		}
		else  // z^2 + w^2 >= x^2 + y^2
		{
			const real sum10 = r[1][1] + r[0][0];
			const real opr22 = _1 + r22;
			if (sum10 <= 0)  // z^2 >= w^2
			{
				const real fourzsqr = opr22 - sum10;
				const real inv4z = _0_5 / sqrt(fourzsqr);
				q.x = (r[2][0] + r[0][2]) * inv4z;
				q.y = (r[2][1] + r[1][2]) * inv4z;
				q.z = fourzsqr * inv4z;
				q.w = (r[1][0] - r[0][1]) * inv4z;
			}
			else  // w^2 >= z^2
			{
				const real fourwsqr = opr22 + sum10;
				const real inv4w = _0_5 / std::sqrt(fourwsqr);
				q.x = (r[2][1] - r[1][2]) * inv4w;
				q.y = (r[0][2] - r[2][0]) * inv4w;
				q.z = (r[1][0] - r[0][1]) * inv4w;
				q.w = fourwsqr * inv4w;
			}
		}

		return q;
	}

	template<typename T>
	quaternion<T> decompose_rotation_matrix(const tmat<T, 3, 3>& r)
	{
        quaternion<T> q;
        constexpr auto _1 = T{ 1 };
        constexpr auto _2 = T{ 2 };
        constexpr auto _0_25 = T{ 0.25 };
        constexpr auto _0_5 = T{ 0.5 };

        // from geometrictools
        auto r22 = r[2][2];
        if (r22 <= 0)  // x^2 + y^2 >= z^2 + w^2
        {
            real dif10 = r[1][1] - r[0][0];
            real omr22 = _1 - r22;
            if (dif10 <= 0)  // x^2 >= y^2
            {
                real fourxsqr = omr22 - dif10;
                real inv4x = _0_5 / sqrt(fourxsqr);
                q.x = fourxsqr * inv4x;
                q.y = (r[1][0] + r[0][1]) * inv4x;
                q.z = (r[2][0] + r[0][2]) * inv4x;
                q.w = (r[2][1] - r[1][2]) * inv4x;
            }
            else  // y^2 >= x^2
            {
                real fourysqr = omr22 + dif10;
                real inv4y = _0_5 / sqrt(fourysqr);
                q.x = (r[1][0] + r[0][1]) * inv4y;
                q.y = fourysqr * inv4y;
                q.z = (r[2][0] + r[0][2]) * inv4y;
                q.w = (r[2][1] - r[1][2]) * inv4y;
            }
        }
        else  // z^2 + w^2 >= x^2 + y^2
        {
            real sum10 = r[1][1] + r[0][0];
            real opr22 = _1 + r22;
            if (sum10 <= 0)  // z^2 >= w^2
            {
                real fourzsqr = opr22 - sum10;
                real inv4z = _0_5 / sqrt(fourzsqr);
                q.x = (r[2][0] + r[0][2]) * inv4z;
                q.y = (r[2][1] + r[1][2]) * inv4z;
                q.z = fourzsqr * inv4z;
                q.w = (r[1][0] - r[0][1]) * inv4z;
            }
            else  // w^2 >= z^2
            {
                real fourwsqr = opr22 + sum10;
                real inv4w = _0_5 / std::sqrt(fourwsqr);
                q.x = (r[2][1] - r[1][2]) * inv4w;
                q.y = (r[0][2] - r[2][0]) * inv4w;
                q.z = (r[1][0] - r[0][1]) * inv4w;
                q.w = fourwsqr * inv4w;
            }
        }

        return q;
	}
}