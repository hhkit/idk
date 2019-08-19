#include "stdafx.h"
#include "color.h"

namespace idk
{
	constexpr color::color(float r, float g, float b, float a) noexcept
		: as_vec4{r, g, b, a}
	{
	}
	constexpr color::color(const math::vector<float,3>& vec) noexcept
		: as_vec4{vec[0], vec[1], vec[2], 1.f}
	{
	}
	constexpr color::color(const math::vector<float,4>& vec) noexcept
		: as_vec4{vec}
	{
	}
	float& color::operator[](size_t index)
	{
		return as_vec4[index];
	}
	const float& color::operator[](size_t index) const
	{
		return as_vec4[index];
	}

	float* color::begin()
	{
		return as_vec4.begin();
	}

	const float* color::begin() const
	{
		return as_vec4.begin();
	}

	float* color::end()
	{
		return as_vec4.end();
	}

	const float* color::end() const
	{
		return as_vec4.end();
	}

	float* color::data()
	{
		return as_vec4.data();
	}

	const float* color::data() const
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

	color& color::operator*=(float rhs)
	{
		for (auto& e : *this)
			e *= rhs;
		return *this;
	}

	constexpr color::operator math::vector<float,3>() const noexcept
	{
		return as_vec3;
	}

	constexpr color::operator math::vector<float,4>() const noexcept
	{
		return as_vec4;
	}
}