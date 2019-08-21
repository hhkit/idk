#pragma once
#include <idk.h>

namespace idk
{
	template<typename T>
	struct Spline
	{
		// accessors
		virtual T val_at  (real t) const = 0;
		virtual T slope_at(real t) const = 0;
		virtual T accel_at(real t) const {};

		// modifiers
		virtual void emplace(const T& val, real t) = 0;
	};

	template<typename T>
	struct BakedSpline : Spline<T>
	{
	private:
		void emplace(const T& val, real t) {};
	};
}