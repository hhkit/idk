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
		retval.position = working[3];
		working[3] = tvec<T, 4>{ 0, 0, 0, 1 };

		retval.scale = vec_t{ working[0].length(), working[1].length(), working[2].length() };

		for (auto i : range<3>())
			working[i] /= retval.scale[i];

		retval.rotation = decompose_rotation_matrix(working);

		return retval;
	}

	template<typename T>
	quaternion<T> decompose_rotation_matrix(const tmat<T, 4, 4> & rotation)
	{
		quaternion<T> retval;
		constexpr auto _1 = T{ 1 };
		constexpr auto _2 = T{ 2 };

		retval.w = sqrt(abs(_1 + rotation[0][0] + rotation[1][1] + rotation[2][2])) / 2;
		const auto w4 = retval.w * 4;
		retval.x = (rotation[2][1] - rotation[1][2]) / w4;
		retval.y = (rotation[0][2] - rotation[2][0]) / w4;
		retval.z = (rotation[1][0] - rotation[0][1]) / w4;

		return retval;
	}

	template<typename T>
	quaternion<T> decompose_rotation_matrix(const tmat<T, 3, 3> & rotation)
	{
		quaternion<T> retval;
		constexpr auto _1 = T{ 1 };
		constexpr auto _2 = T{ 2 };

		retval.w = sqrt(abs(_1 + rotation[0][0] + rotation[1][1] + rotation[2][2])) / 2;
		const auto w4 = retval.w * 4;
		retval.x = (rotation[2][1] - rotation[1][2]) / w4;
		retval.y = (rotation[0][2] - rotation[2][0]) / w4;
		retval.z = (rotation[1][0] - rotation[0][1]) / w4;

		return retval;
	}
}