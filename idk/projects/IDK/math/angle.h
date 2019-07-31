#pragma once

namespace idk
{
	// forward declaration
	template <typename T> struct radian;
	template <typename T> struct degree;

	template <typename T>
	struct radian
	{
		T angle;

		explicit radian(T val = T{});
		explicit radian(const degree<T>&);
		explicit operator degree<T>() const;
	};

	template<typename T>
	struct degree
	{
		T angle;
		explicit degree(T val = T{});
		explicit degree(const radian<T>&);
		explicit operator radian<T>() const;
	};
}

#include "angle.inl"