#include "pch.h"
#include "../IDK/idk.h"
#include <random>

TEST(Math, MatrixConstruction)
{
	using namespace idk;
	idk::mat4 m{
		vec4{1.f,   2.f,  3.f,  4.f},
		vec4{5.f,   6.f,  7.f,  8.f},
		vec4{9.f,  10.f, 11.f, 12.f},
		vec4{13.f, 14.f, 15.f, 16.f}
	};

	idk::mat4 inv_m{
		1.f, 5.f, 9.f,  13.f,
		2.f, 6.f, 10.f, 14.f,
		3.f, 7.f, 11.f, 15.f,
		4.f, 8.f, 12.f, 16.f
	};

	EXPECT_EQ(m, inv_m);
}

TEST(Math, MatrixVectorMultiplication)
{
	using namespace idk;
	idk::mat4 m{
		vec4{2.f, 0.f, 0.f, 0.f},
		vec4{0.f, 2.f, 0.f, 0.f},
		vec4{0.f, 0.f, 2.f, 0.f},
		vec4{0.f, 2.f, 0.f, 1.f}
	};

	idk::mat4 m2{ 
		vec4{2.f, 0.f, 0.f, 0.f}, 
		vec4{0.f, 3.f, 0.f, 0.f}, 
		vec4{0.f, 0.f, 4.f, 0.f}, 
		vec4{0.f, 2.f, 0.f, 1.f}
	};

	EXPECT_EQ(
		(m * idk::vec4{ 1.f, 0.f, 2.f, 0.f }), (idk::vec4{1.f, 0.f, 2.f, 0.f} * 2.f)
	);

	EXPECT_EQ(
		(m2 * idk::vec4{ 1.f, 0.f, 2.f, 0.f }), (idk::vec4{ 2.f, 0.f, 8.f, 0.f })
	);
}

TEST(Math, MatrixTranspose)
{
	using namespace idk;
	idk::mat4 m{
		vec4{1.f, 5.f, 9.f , 13.f},
		vec4{2.f, 6.f, 10.f, 14.f},
		vec4{3.f, 7.f, 11.f, 15.f},
		vec4{4.f, 8.f, 12.f, 16.f}
	};

	idk::mat4 transpose_m{
		1.f, 5.f, 9.f , 13.f,
		2.f, 6.f, 10.f, 14.f,
		3.f, 7.f, 11.f, 15.f,
		4.f, 8.f, 12.f, 16.f
	};

	EXPECT_EQ(m.transpose(), transpose_m);
}

TEST(Math, MatrixMatrixMultiplication)
{
	using namespace idk;
	idk::mat4 m{
		vec4{2.f, 0.f, 0.f, 0.f},
		vec4{0.f, 2.f, 0.f, 0.f},
		vec4{0.f, 0.f, 2.f, 0.f},
		vec4{0.f, 2.f, 0.f, 1.f}
	};
	idk::mat4 m2{
		vec4{2.f, 0.f, 0.f, 0.f},
		vec4{0.f, 3.f, 0.f, 0.f},
		vec4{0.f, 0.f, 4.f, 0.f},
		vec4{0.f, 2.f, 0.f, 1.f}
	};

	idk::mat4 trams_m = m.transpose();

	EXPECT_NE(
		m * trams_m, trams_m * m
	) << "Test for commutativity of scaling matrices failed.";
}

TEST(Math, MatrixDeterminant)
{
	using namespace idk;
	std::default_random_engine generator;
	std::uniform_real_distribution<float> dist(0.f, 100.f);

	for (int i = 0; i < 1000; ++i)
	{
		auto f = dist(generator);

		EXPECT_EQ((idk::mat3{
			f,   0.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 0.f, 1.f
			}.determinant()), f) << "Basic scaling test 1 failed";
		EXPECT_EQ((idk::mat3{
			f,   0.f, 0.f,
			0.f, f,   0.f,
			0.f, 0.f, 1.f
			}.determinant()), f * f) << "Basic scaling test 2 failed";
	}

	EXPECT_EQ((idk::mat3{
		2.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 0.f
	}.determinant()), 0.f) << "Zero column test failed";

	EXPECT_EQ((idk::mat3{
		2.f, 0.f, 0.f,
		0.f, 1.f, 1.f,
		0.f, 0.f, 0.f
	}.determinant()), 0.f) << "Projected column test failed";

}