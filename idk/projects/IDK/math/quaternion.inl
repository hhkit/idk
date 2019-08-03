#pragma once
#include <utility>
#include "quaternion.h"

namespace idk::math
{
	template<typename T>
	quaternion<T> quaternion<T>::inverse() const
	{
		return quaternion{-x, -y, -z, w};
	}
	template<typename T>
	quaternion<T>& quaternion<T>::operator*=(const quaternion& rhs)
	{
		return *this = (*this * rhs);
	}
	template<typename T>
	quaternion<T> quaternion<T>::operator*(const quaternion& rhs) const
	{
		auto copy = *this;
		const auto plus = T{ 1.0 };
		const auto minus = T{ -1.0 };
		copy.x = (this->wxyz * rhs.wxyz).dot(Base{ plus, minus, minus, minus });
		copy.y = (this->wxyz * rhs.xwzy).dot(Base{ plus, plus, plus, minus });
		copy.z = (this->wxyz * rhs.yzwx).dot(Base{ plus, minus, plus, plus });
		copy.w = (this->wxyz * rhs.zyxw).dot(Base{ plus, plus, minus, plus });
		return copy;
	}
	
	template<typename T>
	quaternion<T>::operator matrix<T, 3, 3>() const
	{
		auto copy = *this;
		return copy.operator matrix<T, 3, 3>();
	}

	template<typename T>
	quaternion<T>::operator matrix<T, 3, 3>()
	{
		this->normalize();
		return matrix<T, 3, 3>
		{
			1 - 2 * (y * y + z * z),	2 * (x * y - z * w),		2 * (x * z + y * w),
			2 * (x * y + z * w),		1 - 2 * (x * x + z * z),	2 * (y * z - x * w),
			2 * (x * z - y * w),		2 * (y * z + x * w),		1 - 2 * (y * y + z * z)	
		};
	}

	template<typename M, typename T>
	auto quat_cast(quaternion<T>& q)
	{
		return s_cast<M>(q);
	}

	template<typename M, typename T>
	auto quat_cast(quaternion<T>&& q)
	{
		return s_cast<M>(q);
	}

	template<typename M, typename T>
	auto quat_cast(const quaternion<T>& q)
	{
		return s_cast<M>(q);
	}
}