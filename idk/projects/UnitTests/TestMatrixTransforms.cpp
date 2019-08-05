
#include "pch.h"
#include <idk.h>
#include <math/matrix_transforms.h>

TEST(Matrix, MatrixTransforms)
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