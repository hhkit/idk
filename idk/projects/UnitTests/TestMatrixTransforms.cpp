
#include "pch.h"
#include <idk.h>
#include <math/matrix_transforms.h>

TEST(Matrix, MatrixIdentity)
{
	using namespace idk;
	mat2 expected2
	{
		1.f, 0.f,
		0.f, 1.f
	};

	mat3 expected3
	{
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f
	};

	mat4 expected4
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	EXPECT_EQ(expected2, mat2{});
	EXPECT_EQ(expected3, mat3{});
	EXPECT_EQ(expected4, mat4{});
}

TEST(Matrix, MatrixScale)
{
	using namespace idk;
	mat3 expected
	{
		3.f, 0.f, 0.f,
		0.f, 5.f, 0.f,
		0.f, 0.f, 7.f
	};
	vec3 scale_vec{ 3.f, 5.f, 7.f };

	EXPECT_EQ(scale(scale_vec), expected);
}
TEST(Matrix, MatrixRotate)
{
	using namespace idk;

	auto rot = rotate(vec3{ 1.f, 0.f, 0.f }, rad{ 1.57f });

}