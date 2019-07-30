#include "pch.h"
#include "../IDK/idk.h"

TEST(TestCaseName, TestName) {
	EXPECT_EQ(idk::Vec2{}.dot(idk::Vec2{}), 0);
	EXPECT_EQ(idk::Vec2(), idk::Vec2( 0, 0 ));
	EXPECT_TRUE(
		(idk::Vec4( idk::Vec2(0, 1), idk::Vec2(2,3) )) == idk::Vec4( 0.f, 1.f, 2.f, 3.f )
	);

	EXPECT_TRUE(
		(idk::Vec4(idk::Vec3(2.f, 3.f, 1.f), 1.f)) == idk::Vec4(2.f, 3.f, 1.f, 1.f)
	);
	EXPECT_TRUE((idk::Vec2{1, 2}.x == idk::Vec2{1, 2}[0]));
	EXPECT_EQ(idk::Vec3(1, 0, 0).cross(idk::Vec3(0,1,0)), idk::Vec3(0, 0, 1));
}