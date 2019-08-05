#pragma once
#include "matrix.h"
#include "vector.h"
#include "angle.h"

namespace idk::math
{
	template<typename T>
	T identity();
	
	template<typename T, unsigned D>
	matrix<T, D, D> scale(const vector<T, D>&);

	template<typename T>
	matrix<T, 3, 3> rotate(const vector<T, 3> & axis, radian<T> angle);

	template<typename T, unsigned D>
	matrix<T, D + 1, D + 1> trans(const vector<T, D> & trans);
}
#include "matrix_transforms.inl"