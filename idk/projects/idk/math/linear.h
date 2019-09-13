#pragma once
#include <utility>

#include <meta/tag.h>

namespace idk
{
	template<typename Vector, typename Field>
	struct linear;

	// figure this out later
	template<typename Vector, typename Field,
		typename = std::enable_if_t<
			has_tag_v<Vector, linear> && 
			std::is_convertible_v<typename Vector::Scalar, Field>
		>
	>
	Vector operator*(Field, const Vector&);

	template<typename Vector, typename Field>
	struct linear
	{
		using Scalar = Field;

		constexpr Vector& operator+=(const Vector&);
		constexpr Vector& operator-=(const Vector&);
		constexpr Vector& operator*=(Field);
		constexpr Vector& operator/=(Field);
		constexpr Vector& operator*=(const Vector&);
		constexpr Vector& operator/=(const Vector&);

		constexpr Vector operator+(const Vector&) const;
		constexpr Vector operator-(const Vector&) const;
		constexpr Vector operator-() const;
		constexpr Vector operator*(Field) const;
		constexpr Vector operator/(Field) const;
		constexpr Vector operator*(const Vector&) const;
		constexpr Vector operator/(const Vector&) const;

		constexpr bool operator==(const Vector&) const;
		constexpr bool operator!=(const Vector&) const;
	private:
		constexpr Vector& me();
		constexpr const Vector& me() const;
	};

}
#include "linear.inl"