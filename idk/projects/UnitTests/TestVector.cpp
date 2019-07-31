#include "pch.h"
#include "../IDK/idk.h"

TEST(Math, VectorConstruction) {
	EXPECT_EQ(idk::vec2(), idk::vec2( 0.f, 0.f )) << "Default constructor should initialize to zero.";
	EXPECT_TRUE(
		(idk::vec4( idk::vec2(0.f, 1.f), idk::vec2(2.f,3.f) )) == idk::vec4( 0.f, 1.f, 2.f, 3.f )
	);

	EXPECT_TRUE(
		(idk::vec4(idk::vec3(2.f, 3.f, 1.f), 1.f)) == idk::vec4(2.f, 3.f, 1.f, 1.f)
	);
}

TEST(Math, VectorAccess) {
	EXPECT_TRUE((idk::vec2{ 1.f, 2.f }.x == idk::vec2{ 1.f, 2.f } [0] ));
	EXPECT_TRUE((idk::vec2{ 1.f, 2.f }.y == idk::vec2{ 1.f, 2.f } [1] ));
	EXPECT_TRUE((idk::vec2{ 1.f, 2.f }.x == idk::vec2{ 1.f, 3.f } [0] ));
	EXPECT_TRUE((idk::vec2{ 1.f, 2.f }.y == idk::vec2{ 4.f, 2.f } [1] ));
}

TEST(Math, VectorNormalize)
{
	EXPECT_TRUE(
		(idk::vec2{ 3.f,4.f }.magnitude() == 5.f)
	) << "Test for pythagorean triplet failed";
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