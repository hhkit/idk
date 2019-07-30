#include "stdafx.h"

int test()
{
	idk::Vec2 u{2,3};
	idk::Vec2 v{ 5,6 };
	idk::Vec4 {u, v};
	auto tup = std::make_tuple(1.f, 2.f, 3.f);
	idk::Vec3 v3{ std::get<0>(tup), std::get<1>(tup), std::get<2>(tup) };
	auto vn = idk::detail::TupleToVector<float>(tup, std::make_index_sequence<3>{});
	static_assert(idk::detail::Dim<idk::Vec3>::value == 3, "hah");
	idk::Vec4 v4{ 1.f,2.f, u };

	return static_cast<int>(v.dot(idk::Vec2{}));
}