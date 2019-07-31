#pragma once

#include "constants.h"
#include "angle.h"

namespace idk
{
	template<typename T>
	inline radian<T>::radian(T val)
		: angle{ val }
	{
	}
	template<typename T>
	inline radian<T>::radian(const degree<T>& rhs)
		: angle{ rhs.angle / static_cast<T>(180) * pi()}
	{
	}

	template<typename T>
	radian<T>::operator degree<T>() const
	{
		return angle / pi() * static_cast<T>(180);
	}

	template<typename T>
	inline degree<T>::degree(T val)
	{
	}
	template<typename T>
	inline degree<T>::degree(const radian<T>& rhs)
		: angle{ rhs.angle / pi() * static_cast<T>(180) }
	{
	}
	template<typename T>
	inline degree<T>::operator radian<T>() const
	{
		return angle * pi() / static_cast<T>(180);
	}
}