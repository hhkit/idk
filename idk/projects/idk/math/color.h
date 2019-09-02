#pragma once
#include <math/vector.h>

namespace idk
{
	struct color
	{
		union
		{
			struct { float r, g, b, a; };
			tvec<float, 3> as_vec3;
			tvec<float, 4> as_vec4;
		};

		constexpr color(void) noexcept;
		constexpr color(float fill) noexcept;
		constexpr color(float r, float g, float b, float a = float{ 1 }) noexcept;
		constexpr explicit color(const tvec<float, 3>&) noexcept;
		constexpr explicit color(const tvec<float, 4>&) noexcept;

		// accessors
		constexpr float& operator[](size_t) noexcept;
		constexpr const float& operator[](size_t) const noexcept;

		// iterator
		constexpr float*       begin() noexcept;
		constexpr const float* begin() const noexcept;
		constexpr float*       end() noexcept;
		constexpr const float* end() const noexcept;
		constexpr float*       data() noexcept;
		constexpr const float* data() const noexcept;

		// operator overloads
		constexpr color& operator+=(const color&) noexcept;
		constexpr color& operator*=(const color&) noexcept;
		constexpr color& operator*=(float) noexcept;

		// conversion operators
		constexpr explicit operator tvec<float,3>() const noexcept;
		constexpr explicit operator tvec<float,4>() const noexcept;
	};
}
#include "color.inl"