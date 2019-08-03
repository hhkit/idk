#pragma once
#include "vector.h"
#include "matrix.h"

namespace idk::math
{
	template <typename T>
	struct quaternion : vector<T, 4>
	{
		using Base = vector<T, 4>;
		using Base::Base;

		// conversion to rotation matrix
		explicit operator matrix<T, 3, 3>();
		explicit operator matrix<T, 3, 3>() const;
		explicit operator matrix<T, 4, 4>();
		explicit operator matrix<T, 4, 4>() const;


	};
}