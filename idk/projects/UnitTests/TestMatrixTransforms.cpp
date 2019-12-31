
#include "pch.h"

#include <random>

#include <idk.h>
#include <math/matrix_transforms.inl>
#include <math/matrix_decomposition.inl>

TEST(Math, MatrixIdentity)
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

TEST(Math, MatrixScale)
{
	using namespace idk; 
	std::default_random_engine gen;
	std::uniform_real_distribution<float> dist(0.f, 100.f);

	for (int i = 0; i < 100; ++i)
	{
		vec3 scale_vec{ dist(gen), dist(gen), dist(gen) };
		
		mat4 expected
		{
			scale_vec.x, 0.f, 0.f, 0.f,
			0.f, scale_vec.y, 0.f, 0.f,
			0.f, 0.f, scale_vec.z, 0.f,
            0.f, 0.f, 0.f, 1.0f
		};

		EXPECT_EQ(scale(scale_vec), expected);
	}
}
TEST(Math, MatrixRotate)
{
	using namespace idk;

	auto rot = rotate(vec3{ 1.f, 0.f, 0.f }, rad{ 1.57f });
	auto r2 = rotate(vec3{ 0,1,0 }, rad{ half_pi });

}
TEST(Math, MatrixTranslate)
{
	using namespace idk;

	std::default_random_engine gen;
	std::uniform_real_distribution<float> dist(0.f, 100.f);

	for (int i = 0; i < 100; ++i)
	{
		vec3 trans_vec{ dist(gen), dist(gen), dist(gen) };

		mat4 expected
		{
			1.f, 0.f, 0.f, trans_vec.x,
			0.f, 1.f, 0.f, trans_vec.y,
			0.f, 0.f, 1.f, trans_vec.z,
			0.f, 0.f, 0.f, 1.f
		};

		EXPECT_EQ(translate(trans_vec), expected);
	}
}
TEST(Math, MatrixProject)
{
	using namespace idk;

	auto pm = perspective(deg{ 90 }, 1.f, 0.1f, 100.f);
	auto om = ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
}

TEST(Math, MatrixOrthonormalize)
{
	using namespace idk;

	EXPECT_EQ(
		orthonormalize(
		mat3(
			1, 2, 3,
			0, 1, 2,
			0, 0, 1
		)), 
		mat3()
	);

	EXPECT_EQ(
		orthonormalize(
			mat3(
				3, 2, 3,
				0, 5, 6,
				0, 0, 7
			)),
		mat3()
	);
}

TEST(Math, MatrixDecomposition)
{
	using namespace idk;
	
	matrix_decomposition<real> decompost;
	decompost.position = vec3{ 1,1,1 };
	decompost.scale = vec3{ 2,3,4 };
	decompost.rotation = quat{ vec3{1,0,0}, deg{270} };

	auto m      = decompost.recompose();
	auto decomp = decompose(m);
	
	for (auto& [lcol, rcol] : zip(m, decomp.recompose()))
		for (auto& [elem, erem] : zip(lcol, rcol))
			EXPECT_LE(abs(elem - erem), epsilon * 4);
}