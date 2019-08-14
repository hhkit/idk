#include "stdafx.h"
#include "color.h"

namespace idk
{
	constexpr color::color() noexcept
		: as_vec4{}
	{
	}
	constexpr color::color(real r, real g, real b, real a) noexcept
		: as_vec4{r, g, b, a}
	{
	}
	constexpr color::color(const vec3& vec) noexcept
		: as_vec3{vec}
	{
	}
	constexpr color::color(const vec4& vec) noexcept
		: as_vec4{vec}
	{
	}
	real& color::operator[](size_t index)
	{
		return as_vec4[index];
	}
	const real& color::operator[](size_t index) const
	{
		return as_vec4[index];
	}

	real* color::begin()
	{
		return as_vec4.begin();
	}

	const real* color::begin() const
	{
		return as_vec4.begin();
	}

	real* color::end()
	{
		return as_vec4.end();
	}

	const real* color::end() const
	{
		return as_vec4.end();
	}

	real* color::data()
	{
		return as_vec4.data();
	}

	const real* color::data() const
	{
		return as_vec4.data();
	}

	color& color::operator+=(const color& col)
	{
		for (auto [lhs, rhs] : zip(*this, col))
			lhs += rhs;
		return *this;
	}

	color& color::operator*=(const color& col)
	{
		for (auto [lhs, rhs] : zip(*this, col))
			lhs *= rhs;
		return *this;
	}

	color& color::operator*=(real rhs)
	{
		for (auto& e : *this)
			e *= rhs;
		return *this;
	}

	constexpr color::operator vec3() const noexcept
	{
		return as_vec3;
	}

	constexpr color::operator vec4() const noexcept
	{
		return as_vec4;
	}
}