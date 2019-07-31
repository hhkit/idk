#include "stdafx.h"

int test()
{
	using namespace idk;
	idk::vec2 u{2.f,3.f};
	idk::vec2 v{ 5.f,6.f };
	idk::vec4 {u, v};
	auto tup = std::make_tuple(1.f, 2.f, 3.f);
	idk::vec3 v3{ std::get<0>(tup), std::get<1>(tup), std::get<2>(tup) };
	idk::vec4 v3_homogenous{ v3, 1.f };
	idk::vector<float, 1> v1 = 1.f;

	idk::mat4 m{
		vec4{1.f, 0.f, 2.f, 3.f},
		vec4{1.f, 0.f, 2.f, 3.f},
		vec4{1.f, 0.f, 2.f, 3.f},
		vec4{1.f, 0.f, 2.f, 3.f} 
	};

	//auto i = idk::detail::MatrixTransposeRow<0>(m, std::make_index_sequence<4>{});

	idk::vector<float, 4> vm {
		(m[0][0]), (m[1][0]), (m[2][0]), (m[3][0])
	};
	return static_cast<int>(v.dot(idk::vec2{}));
}