#pragma once
#include <idk.h>

namespace idk
{
	struct color
	{
		union
		{
			struct { real r, g, b, a; };
			vec3 as_vec3;
			vec4 as_vec4;
		};

		constexpr color() noexcept;
		constexpr color(real r, real g, real b, real a = real{ 1 }) noexcept;
		constexpr explicit color(const vec3&) noexcept;
		constexpr explicit color(const vec4&) noexcept;

		// accessors
		real& operator[](size_t);
		const real& operator[](size_t) const;

		// iterator
		real*       begin();
		const real* begin() const;
		real*       end();
		const real* end() const;
		real*       data();
		const real* data() const;

		// operator overloads
		color& operator+=(const color&);
		color& operator*=(const color&);
		color& operator*=(real);

		// conversion operators
		constexpr explicit operator vec3() const noexcept;
		constexpr explicit operator vec4() const noexcept;
	};
}