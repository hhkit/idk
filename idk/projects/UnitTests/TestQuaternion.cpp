#include "pch.h"
#include "../IDK/idk.h"

#include <math/matrix_transforms.h>
#include <math/matrix_decomposition.h>

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

TEST(Math, QuaternionNegatives)
{
	//idk::quat q{ idk::vec4{} };
	idk::quat q2{ 1.f, 2.f, 3.f, 4.f };
	for (auto& elem : q2)
	{
		elem += 5.f;
	}

	idk::mat4 test4;
	test4[0] = idk::vec4{ -1,-0,0,0 };
	test4[1] = idk::vec4{ -0,-1,0,0 };
	test4[2] = idk::vec4{ -0,-0,1,0 };
	test4[3] = idk::vec4{ -0,-0,0,1 };
	
	auto chk = idk::decompose_rotation_matrix(test4);
	auto decomp = idk::decompose(idk::mat4{});
	decomp.rotation = chk;
	idk::mat4 finalmat = decomp.recompose();

	EXPECT_FALSE(isnan(finalmat[0].x));

	idk::mat3 test3;
	test3[0] = idk::vec3{ -1,-0,0 };
	test3[1] = idk::vec3{ -0,-1,0 };
	test3[2] = idk::vec3{ -0,-0,1 };

	chk = idk::decompose_rotation_matrix(test3);

	//EXPECT_FALSE(isnan(test3[0].x));	
}