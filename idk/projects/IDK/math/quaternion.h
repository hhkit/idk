#pragma once
#include "angle.h"
#include "vector.h"
#include "matrix.h"

namespace idk::math
{
	template <typename T>
	struct quaternion
		: vector<T, 4>
	{
		using Base = vector<T, 4>;
		using Base::x;
		using Base::y;
		using Base::z;
		using Base::w;

		quaternion();
		quaternion(T x, T y, T z, T w);
		quaternion(const vector<T, 3> & axis, radian<T> angle);

		quaternion inverse() const;

		// operator overloads
		quaternion& operator*=(const quaternion&);
		quaternion  operator*(const quaternion&) const;

		// conversion to rotation matrix
		explicit operator matrix<T, 3, 3>();
		explicit operator matrix<T, 3, 3>() const;
		explicit operator matrix<T, 4, 4>();
		explicit operator matrix<T, 4, 4>() const;
	};
}

namespace idk
{
	template<typename M, typename T> auto quat_cast(math::quaternion<T>& q);
	template<typename M, typename T> auto quat_cast(math::quaternion<T>&& q);
	template<typename M, typename T> auto quat_cast(const math::quaternion<T>& q);


	extern template struct math::quaternion<float>;
}

#include "quaternion.inl"