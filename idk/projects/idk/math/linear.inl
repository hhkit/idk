#pragma once
#include <ds/zip.h>
#include <meta/casts.h>
#include "linear.h"
namespace idk
{
	template<typename Vector, typename Field>
	constexpr Vector& linear<Vector, Field>::operator+=(const Vector& rvec)
	{
		auto itr = me().begin();
		const auto end = me().end();
		auto rtr = rvec.begin();
		while (itr != end)
			* itr++ += *rtr++;
		return me();
	}

	template<typename Vector, typename Field>
	constexpr Vector& linear<Vector, Field>::operator-=(const Vector& rvec)
	{
		for (auto [lhs, rhs] : zip(me().values, rvec.values))
			lhs -= rhs;
		return me();
	}

	template<typename Vector, typename Field>
	constexpr Vector& linear<Vector, Field>::operator*=(Field scalar)
	{
		for (auto& lhs : me().values)
			lhs *= scalar;
		return me();
	}

	template<typename Vector, typename Field>
	constexpr Vector& linear<Vector, Field>::operator/=(Field scalar)
	{
		for (auto& lhs : me().values)
			lhs /= scalar;
		return me();
	}

	template<typename Vector, typename Field>
	constexpr Vector& linear<Vector, Field>::operator*=(const Vector& rvec)
	{
		auto itr = me().begin();
		const auto end = me().end();
		auto rtr = rvec.begin();
		while (itr != end)
			*itr++ *= *rtr++;

		return me();
	}

	template<typename Vector, typename Field>
	constexpr Vector& linear<Vector, Field>::operator/=(const Vector& rvec)
	{
		for (auto [lhs, rhs] : zip(me().values, rvec.values))
			lhs /= rhs;
		return me();
	}

	template<typename Vector, typename Field>
	constexpr Vector linear<Vector, Field>::operator+(const Vector& rhs) const
	{
		auto copy = me();
		return copy += rhs;
	}

	template<typename Vector, typename Field>
	constexpr Vector linear<Vector, Field>::operator-(const Vector& rhs) const
	{
		auto copy = me();
		return copy -= rhs;
	}

	template<typename Vector, typename Field>
	constexpr Vector linear<Vector, Field>::operator-() const
	{
		auto copy = me();
		for (auto& elem : copy.values)
			elem = -elem;
		return copy;
	}

	template<typename Vector, typename Field>
	constexpr Vector linear<Vector, Field>::operator*(Field rhs) const
	{
		auto copy = me();
		return copy.linear<Vector, Field>::operator*=(rhs);
	}

	template<typename Vector, typename Field>
	constexpr Vector linear<Vector, Field>::operator/(Field rhs) const
	{
		auto copy = me();
		return copy /= rhs;
	}

	template<typename Vector, typename Field>
	constexpr Vector linear<Vector, Field>::operator*(const Vector& rhs) const
	{
		auto copy = me();
		return copy *= rhs;
	}

	template<typename Vector, typename Field>
	constexpr Vector linear<Vector, Field>::operator/(const Vector& rhs) const
	{
		auto copy = me();
		return copy /= rhs;
	}

	template<typename Vector, typename Field>
	inline constexpr bool linear<Vector, Field>::operator==(const Vector& rvec) const
	{
		for (auto [lhs, rhs] : zip(me().values, rvec.values))
			if (lhs != rhs)
				return false;
		return true;
	}

	template<typename Vector, typename Field>
	inline constexpr bool linear<Vector, Field>::operator!=(const Vector& rhs) const
	{
		return !operator==(rhs);
	}

	template<typename Vector, typename Field, typename>
	Vector operator*(Field coeff, const Vector& v)
	{
		return v * coeff;
	}

	template<typename Vector, typename Field>
	constexpr Vector& linear<Vector, Field>::me()
	{
		return *r_cast<Vector*>(this);
	}

	template<typename Vector, typename Field>
	constexpr const Vector& linear<Vector, Field>::me() const
	{
		return *r_cast<const Vector*>(this);
	}
}