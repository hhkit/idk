#pragma once
#include "matrix.h"
#include "vector.h"
#include "angle.h"

namespace idk
{
	template<typename T>
	T identity();
	
	template<typename T, unsigned D>
	math::matrix<T, D, D> scale(const math::vector<T, D>&);

	template<typename T>
	math::matrix<T, 3, 3> rotate(const math::vector<T, 3> & axis, math::radian<T> angle);

	template<typename T, unsigned D>
	math::matrix<T, D + 1, D + 1> translate(const math::vector<T, D> & translate);
}
#include "matrix_transforms.inl"