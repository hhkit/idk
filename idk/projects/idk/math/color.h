#pragma once
#include <math/vector.h>

namespace idk
{
	struct color
	{
		union
		{
			struct { float r, g, b, a; };
			math::vector<float, 3> as_vec3;
			math::vector<float, 4> as_vec4;
		};

		constexpr color(void) noexcept : r{}, g{}, b{}, a{ 1.f } {}
		constexpr color(float r, float g, float b, float a = float{ 1 }) noexcept;
		constexpr explicit color(const math::vector<float, 3>&) noexcept;
		constexpr explicit color(const math::vector<float, 4>&) noexcept;

		// accessors
		float& operator[](size_t);
		const float& operator[](size_t) const;

		// iterator
		float*       begin();
		const float* begin() const;
		float*       end();
		const float* end() const;
		float*       data();
		const float* data() const;

		// operator overloads
		color& operator+=(const color&);
		color& operator*=(const color&);
		color& operator*=(float);

		// conversion operators
		constexpr explicit operator math::vector<float,3>() const noexcept;
		constexpr explicit operator math::vector<float,4>() const noexcept;
	};
}