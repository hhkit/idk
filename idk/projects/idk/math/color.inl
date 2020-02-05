#pragma once
#include "color.h"

namespace idk
{
	constexpr color::color(void) noexcept
		: r{}, g{}, b{}, a{ 1.f }
	{}
	inline constexpr color::color(float fill) noexcept
		: color {fill, fill, fill}
	{
	}
	constexpr color::color(float r, float g, float b, float a) noexcept
		: as_vec4{ r, g, b, a }
	{
	}
	constexpr color::color(const tvec<float, 3> & vec) noexcept
		: as_vec4{ vec[0], vec[1], vec[2], 1.f }
	{
	}
	constexpr color::color(const tvec<float, 4> & vec) noexcept
		: as_vec4{ vec }
	{
	}
	constexpr color::color(const float* rgba) noexcept
		: as_vec4{ rgba }
	{
	}
	constexpr float& color::operator[](size_t index) noexcept
	{
		return as_vec4[index];
	}
	constexpr const float& color::operator[](size_t index) const noexcept
	{
		return as_vec4[index];
	}

	constexpr float* color::begin() noexcept
	{
		return as_vec4.begin();
	}

	constexpr const float* color::begin() const noexcept
	{
		return as_vec4.begin();
	}

	constexpr float* color::end() noexcept
	{
		return as_vec4.end();
	}

	constexpr const float* color::end() const noexcept
	{
		return as_vec4.end();
	}

	constexpr float* color::data() noexcept
	{
		return as_vec4.data();
	}

	constexpr const float* color::data() const noexcept
	{
		return as_vec4.data();
	}

	constexpr color& color::operator+=(const color& col) noexcept
	{
		for (auto [lhs, rhs] : zip(*this, col))
			lhs += rhs;
		return *this;
	}

	constexpr color& color::operator*=(const color& col) noexcept
	{
		for (auto [lhs, rhs] : zip(*this, col))
			lhs *= rhs;
		return *this;
	}

	constexpr color& color::operator*=(float rhs) noexcept
	{
		for (auto& e : *this)
			e *= rhs;
		return *this;
	}

	constexpr color::operator tvec<float, 3>() const noexcept
	{
		return as_vec3;
	}

	constexpr color::operator tvec<float, 4>() const noexcept
	{
		return as_vec4;
	}
}