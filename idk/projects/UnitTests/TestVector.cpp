#include "pch.h"
#include "../IDK/idk.h"
#include <random>

TEST(Math, VectorConstruction) {
	EXPECT_EQ(idk::vec2(), idk::vec2( 0.f, 0.f )) << "Default constructor should initialize to zero.";
	EXPECT_TRUE(
		idk::vec4( idk::vec2(0.f, 1.f), idk::vec2(2.f,3.f) ) == idk::vec4( 0.f, 1.f, 2.f, 3.f )
	);

	EXPECT_TRUE(
		idk::vec4(idk::vec3(2.f, 3.f, 1.f), 1.f) == idk::vec4(2.f, 3.f, 1.f, 1.f)
	);

	EXPECT_TRUE(
		idk::vec3(idk::vec4(2.f, 3.f, 1.f, 1.f)) == idk::vec3(2.f, 3.f, 1.f)
	);
}

template<typename vec>
void TestNormalized()
{
	bool res;
	float len = vec{ 0.5f }.get_normalized().length();
	res = abs(len - 1) <= idk::constants::epsilon<float>();
	EXPECT_TRUE(res);
	len = vec{ 5.5f }.get_normalized().length();
	res = abs(len - 1) <= idk::constants::epsilon<float>();
	EXPECT_TRUE(res);
	len = vec{ 99.5f }.get_normalized().length();
	res = abs(len - 1) <= idk::constants::epsilon<float>();
	EXPECT_TRUE(res);
	len = vec{ 0.015f }.get_normalized().length();
	res = abs(len - 1) <= idk::constants::epsilon<float>();
	EXPECT_TRUE(res);
	len = vec{ idk::constants::epsilon<float>() }.get_normalized().length();
	res = abs(len - 1) <= idk::constants::epsilon<float>();

	len = vec{ -0.5f }.get_normalized().length();
	res = abs(len - 1) <= idk::constants::epsilon<float>();
	EXPECT_TRUE(res);
	len = vec{ -5.5f }.get_normalized().length();
	res = abs(len - 1) <= idk::constants::epsilon<float>();
	EXPECT_TRUE(res);
	len = vec{ -99.5f }.get_normalized().length();
	res = abs(len - 1) <= idk::constants::epsilon<float>();
	EXPECT_TRUE(res);
	len = vec{ -0.015f }.get_normalized().length();
	res = abs(len - 1) <= idk::constants::epsilon<float>();
	EXPECT_TRUE(res);
	len = vec{ -idk::constants::epsilon<float>() }.get_normalized().length();
	res = abs(len - 1) <= idk::constants::epsilon<float>();
	EXPECT_TRUE(res);
}

TEST(Math, VectorGettors) {
	TestNormalized < idk::vec2>();
	TestNormalized < idk::vec3>();
	TestNormalized < idk::vec4>();

}

TEST(Math, VectorAccess) {
	EXPECT_TRUE((idk::vec2{ 1.f, 2.f }.x == idk::vec2{ 1.f, 2.f } [0] ));
	EXPECT_TRUE((idk::vec2{ 1.f, 2.f }.y == idk::vec2{ 1.f, 2.f } [1] ));
	EXPECT_TRUE((idk::vec2{ 1.f, 2.f }.x == idk::vec2{ 1.f, 3.f } [0] ));
	EXPECT_TRUE((idk::vec2{ 1.f, 2.f }.y == idk::vec2{ 4.f, 2.f } [1] ));
}

TEST(Math, VectorMagnitudeNormalize)
{
	EXPECT_TRUE(
		(idk::vec2{ 3.f,4.f }.length() == 5.f)
	) << "Test for pythagorean triplet failed";

	std::default_random_engine generator;
	std::uniform_real_distribution<float> dist(-100.f, 100.f);

	&idk::tvec<float, 3>::operator*;

	for (int i = 0; i < 1000; ++i)
	{
		auto f = dist(generator);
		EXPECT_LT(
			abs(idk::vec2{ dist(generator), dist(generator) }.normalize().length() - 1.f), 1e-6f//idk::constants::epsilon<float>()
		);
		EXPECT_LT(
			abs(idk::vec3{ dist(generator), dist(generator), dist(generator) }.normalize().length() - 1.f), 1e-6f//idk::constants::epsilon<float>()
		);
		EXPECT_LT(
			abs(idk::vec4{ dist(generator), dist(generator), dist(generator), dist(generator) }.normalize().length() - 1.f), 1e-6f//idk::constants::epsilon<float>()
		);
	}
}

TEST(Math, VectorDotPdt)
{
	EXPECT_EQ(idk::vec2{}.dot(idk::vec2{}), 0)           << "Trivial dot product failed";
	EXPECT_EQ((idk::vec2{ 1.f, 0.f }.dot(idk::vec2{})), 0)     << "Test for orthogonality with zero vector failed";
	EXPECT_EQ((idk::vec2{ 1.f, 0.f }.dot(idk::vec2{0.f, 1.f})), 0) << "Test for orthogonality failed";
	EXPECT_LT((idk::vec2{ 1.f, 0.f }.dot(idk::vec2{ -1.f,0.f })), 0) << "Test for opposing vectors failed";
}

TEST(Math, VectorCrossPdt)
{
	EXPECT_EQ(idk::vec3(1.f, 0.f, 0.f).cross(idk::vec3(0.f, 1.f, 0.f)), idk::vec3(0.f, 0.f, 1.f));
}

TEST(Math, VectorSwizzle)
{
	using namespace idk;
	vec2 v{ 1.f, 2.f };
	vec2 v2{ 2.f, 1.f };
	vec3 v3{ 1.f, 2.f, 3.f };
	vec4 v4{ 1.f, 2.f, 4.f, 5.f };
	EXPECT_EQ(v, v2.yx);
	EXPECT_EQ(v2, v3.yx);
	v2.yx = v2;
	EXPECT_EQ(v2, (vec2{ 1.f, 2.f }));
	v2.yx = v3.zz;
	EXPECT_EQ(v2, (vec2{ 3.f, 3.f }));
}