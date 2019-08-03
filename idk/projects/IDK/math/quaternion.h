#pragma once
#include "vector.h"
#include "matrix.h"

namespace idk::math
{
	template <typename T>
	struct quaternion 
		: vector<T, 4>
	{
		using Base = vector<T, 4>;
		using Base::Base;
		using Base::x;
		using Base::y;
		using Base::z;
		using Base::w;

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

	template<typename M, typename T> auto quat_cast(quaternion<T>& q);
	template<typename M, typename T> auto quat_cast(quaternion<T>&& q);
	template<typename M, typename T> auto quat_cast(const quaternion<T>& q);
}

#include "quaternion.inl"