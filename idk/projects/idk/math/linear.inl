#pragma once
#include <xmmintrin.h>

#include <ds/zip.h>
#include <meta/casts.h>
#include "linear.h"
namespace idk
{
	template<typename T, unsigned D>
	struct tvec;

	template<typename Vector, typename Field>
	constexpr Vector& linear<Vector, Field>::operator+=(const Vector& rvec)
	{
		if constexpr (std::is_same_v<Vector, tvec<float, 4>>)
		{
			tvec<float, 4> & as_sse = me();
			as_sse.sse = _mm_add_ps(as_sse.sse, rvec.sse);
		}
		else
		{
			auto itr = me().begin();
			const auto end = me().end();
			auto rtr = rvec.begin();
			while (itr != end)
				* itr++ += *rtr++;
		}
		return me();
	}

	template<typename Vector, typename Field>
	constexpr Vector& linear<Vector, Field>::operator-=(const Vector& rvec)
	{
		if constexpr (std::is_same_v<Vector, tvec<float, 4>>)
		{
			tvec<float, 4> & as_sse = me();
			as_sse.sse = _mm_sub_ps(as_sse.sse, rvec.sse);
		}
		else
		{
			auto itr = me().begin();
			const auto end = me().end();
			auto rtr = rvec.begin();
			while (itr != end)
				* itr++ -= *rtr++;
		}
        return me();
	}

	template<typename Vector, typename Field>
	constexpr Vector& linear<Vector, Field>::operator*=(Field scalar)
	{
		if constexpr (std::is_same_v<Vector, tvec<float, 4>>)
		{
			tvec<float, 4> & as_sse = me();
			as_sse.sse = _mm_mul_ps(as_sse.sse, _mm_set1_ps(scalar));
		}
		else
		{
			auto itr = me().begin();
			const auto* end = me().end();
			while (itr != end)
				* itr++ *= scalar;
		}
		return me();
	}

	template<typename Vector, typename Field>
	constexpr Vector& linear<Vector, Field>::operator/=(Field scalar)
	{
		if constexpr (std::is_same_v<Vector, tvec<float, 4>>)
		{
			tvec<float, 4> & as_sse = me();
			as_sse.sse = _mm_div_ps(as_sse.sse, _mm_set1_ps(scalar));
		}
		else
		{
			auto itr = me().begin();
			const auto end = me().end();
			while (itr != end)
				* itr++ /= scalar;
		}
        return me();
	}

	template<typename Vector, typename Field>
	constexpr Vector& linear<Vector, Field>::operator*=(const Vector& rvec)
	{
		if constexpr (std::is_same_v<Vector, tvec<float, 4>>)
		{
			tvec<float, 4> & as_sse = me();
			as_sse.sse = _mm_mul_ps(as_sse.sse, rvec.sse);
		}
		else
		{
			auto itr = me().begin();
			const auto end = me().end();
			auto rtr = rvec.begin();
			while (itr != end)
				* itr++ *= *rtr++;
		}
		return me();
	}

	template<typename Vector, typename Field>
	constexpr Vector& linear<Vector, Field>::operator/=(const Vector& rvec)
	{
		if constexpr (std::is_same_v<Vector, tvec<float, 4>>)
		{
			tvec<float, 4> & as_sse = me();
			as_sse.sse = _mm_div_ps(as_sse.sse, rvec.sse);
		}
		else
		{
			auto itr = me().begin();
			const auto end = me().end();
			auto rtr = rvec.begin();
			while (itr != end)
				* itr++ /= *rtr++;
		}
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
        auto itr = me().begin();
        const auto end = me().end();
        auto rtr = rvec.begin();
        while (itr != end)
            if (*itr++ != *rtr++)
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
		return *reinterpret_cast<Vector*>(this);
	}

	template<typename Vector, typename Field>
	constexpr const Vector& linear<Vector, Field>::me() const
	{
		return *reinterpret_cast<const Vector*>(this);
	}
}