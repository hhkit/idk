#pragma once
#include <math/matrix.h>
#include <math/quaternion.h>

namespace idk
{
	template<typename T>
	struct matrix_decomposition
	{
		tvec<T, 3>  scale;
		quaternion<T> rotation;
		tvec<T, 3>  shear;
		tvec<T, 3>  position;

		tmat<T, 4, 4> recompose() const;
	};

	template<typename T>
	matrix_decomposition<T> decompose(const tmat<T, 4, 4> & mat);

	template<typename T>
	quaternion<T> decompose_rotation_matrix(const tmat<T, 4, 4> & rotation);

	template<typename T>
	quaternion<T> decompose_rotation_matrix(const tmat<T, 3, 3> & rotation);
}
