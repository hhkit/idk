#include "pch.h"
#include "../IDK/idk.h"

TEST(Math, QuaternionConstruction)
{
	//idk::quat q{ idk::vec4{} };
	idk::quat q2{ 1.f, 2.f, 3.f, 4.f };
	for (auto& elem : q2)
	{
		elem += 5.f;
	}

	q2.operator idk::mat3();
	std::is_same_v<idk::quat, float>;
	static_cast<idk::mat3>(q2);
}