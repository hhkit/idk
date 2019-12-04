#pragma once
#include "matrix_decomposition.h"
#include <math/matrix_transforms.h>

namespace idk
{
	template<typename T>
	inline tmat<T, 4, 4> matrix_decomposition<T>::recompose() const
	{
		return translate(this->position) * quat_cast<mat4>(rotation) *idk::scale(this->scale);
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

		retval.rotation = decompose_rotation_matrix(working);

		return retval;
	}

	template<typename T>
	quaternion<T> decompose_rotation_matrix(const tmat<T, 4, 4>& m)
	{
		T fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
		T fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
		T fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
		T fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

		int biggestIndex = 0;
		T fourBiggestSquaredMinus1 = fourWSquaredMinus1;
		if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourXSquaredMinus1;
			biggestIndex = 1;
		}
		if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourYSquaredMinus1;
			biggestIndex = 2;
		}
		if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourZSquaredMinus1;
			biggestIndex = 3;
		}

		T biggestVal = sqrt(fourBiggestSquaredMinus1 + static_cast<T>(1))* static_cast<T>(0.5);
		T mult = static_cast<T>(0.25) / biggestVal;

		switch (biggestIndex)
		{
		case 0: return quaternion<T>((m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult, biggestVal);
		case 1: return quaternion<T>(biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] - m[2][1]) * mult);
		case 2: return quaternion<T>((m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult, (m[2][0] - m[0][2]) * mult);
		case 3: return quaternion<T>((m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal, (m[0][1] - m[1][0]) * mult);
		default: // Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
			assert(false);
			return quaternion<T>(1, 0, 0, 0);
		}
	}

	template<typename T>
	quaternion<T> decompose_rotation_matrix(const tmat<T, 3, 3>& r)
	{
		return decompose_rotation_matrix(tmat<T, 4, 4>(r));
	}
}