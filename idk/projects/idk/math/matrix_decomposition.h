#pragma once
#include <math/matrix.h>
#include <math/quaternion.h>

namespace idk
{
	template<typename T>
	struct matrix_decomposition
	{
		math::vector<T, 3>  scale;
		math::quaternion<T> rotation;
		math::vector<T, 3>  shear;
		math::vector<T, 3>  position;

		math::matrix<T, 4, 4> recompose() const;
	};

	template<typename T>
	matrix_decomposition<T> decompose(const math::matrix<T, 4, 4> & mat);

	template<typename T>
	math::quaternion<T> decompose_rotation_matrix(const math::matrix<T, 4, 4> & rotation);

	template<typename T>
	math::quaternion<T> decompose_rotation_matrix(const math::matrix<T, 3, 3> & rotation);
}

#include "matrix_decomposition.inl"