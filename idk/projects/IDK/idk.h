#pragma once
#include <array>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <stddef.h>
#include <functional>

#include "math/constants.h"
#include "math/arith.h"
#include "math/angle.h"
#include "math/vector.h"
#include "math/matrix.h"

namespace idk
{
	// math
	using real = float;
	using vec2 = math::vector<real, 2>;
	using vec3 = math::vector<real, 3>;
	using vec4 = math::vector<real, 4>;
	
	using mat2 = math::matrix<real, 2, 2>;
	using mat3 = math::matrix<real, 3, 3>;
	using mat4 = math::matrix<real, 4, 4>;

	using rad = math::radian<real>;
	using deg = math::degree<real>;

	// math constants
	constexpr auto pi = math::constants::pi<real>();
	constexpr auto half_pi = pi / 2;
	constexpr auto two_pi = math::constants::tau<real>();

	// containers
	using byte = std::byte;

	template<typename T, size_t N>
	using array = std::array<T, N>;

	template<typename T>
	using vector = std::vector<T>;

	template<typename T1, typename T2>
	using hash_table = std::unordered_map<T1, T2>;

	template<typename T>
	using hash_set = std::unordered_set<T>;

	// utility
	template<typename Signature>
	using function = std::function<Signature>;

	template<typename ... Ts>
	using tuple = std::tuple<Ts...>;
}