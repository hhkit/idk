#pragma once
#include <utility>
#include "quaternion.h"
#include <math/angle.inl>

namespace idk
{
	template<typename T>
	inline quaternion<T>::quaternion()
		: Base{ 0, 0, 0, 1 }
	{
	}
	template<typename T>
	inline quaternion<T>::quaternion(T x, T y, T z, T w)
		: Base{ x, y, z, w }
	{
	}
	template<typename T>
	inline quaternion<T>::quaternion(const tvec<T, 3> & axis, trad<T> angle)
	{
		const auto h = angle / 2;
		const auto s = sin(h);
		const auto c = cos(h);
		const auto n = s * axis.get_normalized();

		this->xyz = n;
		w = c;
	}
	template<typename T>
	inline T quaternion<T>::dot(const quaternion& rhs) const
	{
		return this->Base::dot(rhs);
	}
	template<typename T>
	quaternion<T> quaternion<T>::inverse() const
	{
		return quaternion{ -x, -y, -z, w };
	}
	template<typename T>
	quaternion<T> quaternion<T>::get_normalized() const
	{
		auto copy = *this;
		return copy.normalize();
	}
	template<typename T>
	quaternion<T>& quaternion<T>::normalize()
	{
		return static_cast<quaternion<T>&>(Base::normalize());
	}
	template<typename T>
	quaternion<T>& quaternion<T>::operator*=(const quaternion& rhs)
	{
		return *this = (*this * rhs);
	}
	template<typename T>
	quaternion<T> quaternion<T>::operator*(const quaternion& r) const
	{
		auto copy = *this;
		//const auto plus = T{ 1.0 };
		//const auto minus = T{ -1.0 };
		//copy.w = (this->wxyz * r.wxyz).dot(Base{ plus, minus, minus, minus });
		//copy.x = (this->wxyz * r.xwzy).dot(Base{ plus, plus, plus, minus });
		//copy.y = (this->wxyz * r.yzwx).dot(Base{ plus, minus, plus, plus });
		//copy.z = (this->wxyz * r.zyxw).dot(Base{ plus, plus, minus, plus });
		copy.w = w * r.w - x * r.x - y * r.y - z * r.z;
		copy.x = w * r.x + x * r.w + y * r.z - z * r.y;
		copy.y = w * r.y - x * r.z + y * r.w + z * r.x;
		copy.z = w * r.z + x * r.y - y * r.x + z * r.w;
		return copy;
	}

	template<typename T>
	inline quaternion<T>::operator tmat<T, 4, 4>() const
	{
		auto copy = *this;
		return tmat<T, 4, 4>{copy};
	}

	template<typename T>
	inline quaternion<T>::operator tmat<T, 4, 4>()
	{
		this->normalize();
		// poor attempt at simd
		////1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w),
		////	  2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w),
		////	  2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)
		//
		//__m128& me = this->sse;
		//
		//auto mul1 = _mm_shuffle_ps(me, me, )
		//
		//return tmat<T, 4, 4>{
		//	vec4{ },
		//	vec4{ },
		//	vec4{ },
		//	vec4{ __m128{0,0,0,1} }
		//};
		return tmat<T, 4, 4>{operator tmat<T, 3, 3>()};
	}

	template<typename T>
	quaternion<T>::operator tmat<T, 3, 3>() const
	{
		auto copy = *this;
		return copy.operator tmat<T, 3, 3>();
	}

	template<typename T>
	quaternion<T>::operator tmat<T, 3, 3>()
	{
		this->normalize();
		return tmat<T, 3, 3>
		{
			1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w),
				2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w),
				2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)
		};
	}
}
namespace idk
{
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