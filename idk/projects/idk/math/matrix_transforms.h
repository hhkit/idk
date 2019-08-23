#pragma once
#include "matrix.h"
#include "vector.h"
#include "angle.h"

#undef near
#undef far

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

	template<typename T>
	math::matrix<T, 4, 4> perspective(math::radian<T> fov, T aspect_ratio, T near, T far);

	template<typename T>
	math::matrix<T, 4, 4> perspective(math::degree<T> fov, T aspect_ratio, T near, T far);

	template<typename T>
	math::matrix<T, 4, 4> ortho(T left, T right, T bottom, T top, T near, T far);

	template<typename T>
	math::matrix<T, 4, 4> look_at(const math::vector<T, 3>& eye, const math::vector<T, 3>& object, const math::vector<T, 3>& up);

	// convert a basis to an orthonormal basis using gramm-schidt
	template<typename T, unsigned D>
	math::matrix<T, D, D> orthonormalize(const math::matrix<T, D, D>&);
}
#include "matrix_transforms.inl"