#pragma once
#include <array>
#include <unordered_map>

#include "Vector.h"
#include "Matrix.h"


namespace idk
{
	using real = float;
	using vec2 = vector<real, 2>;
	using vec3 = vector<real, 3>;
	using vec4 = vector<real, 4>;
	
	using mat2 = matrix<real, 2, 2>;
	using mat3 = matrix<real, 3, 3>;
	using mat4 = matrix<real, 4, 4>;

	template<typename T, size_t N>
	using array = std::array<T, N>;

	template<typename T1, typename T2>
	using hash_table = std::unordered_map<T1, T2>;
}