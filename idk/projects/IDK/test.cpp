#include "stdafx.h"

int test()
{
	idk::vec2 u{2,3};
	idk::vec2 v{ 5,6 };
	idk::vec4 {u, v};
	auto tup = std::make_tuple(1.f, 2.f, 3.f);
	idk::vec3 v3{ std::get<0>(tup), std::get<1>(tup), std::get<2>(tup) };
	idk::vec4 v3_homogenous{ v3, 1.f };

	return static_cast<int>(v.dot(idk::vec2{}));
}