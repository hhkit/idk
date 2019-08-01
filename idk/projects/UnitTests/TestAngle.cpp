#include "pch.h"
#include "../IDK/idk.h"

TEST(Math, AngleTest)
{
	idk::rad r{ 3.14159f };
	idk::deg d{ 180 };
	idk::rad r2{ d };
	idk::deg d2{ r };

}

TEST(Math, AngleComparison)
{
	for (int i = 0; i < 100; ++i)
	{
		EXPECT_TRUE ((idk::deg{ 180 }.abs_comp(idk::deg{ 180.f + 360.f * i })));
		EXPECT_FALSE((idk::deg{ 180 }.abs_comp(idk::deg{ 181.f + 360.f * i })));


		EXPECT_TRUE((idk::rad{ idk::pi }.abs_comp(idk::rad{ idk::pi + idk::two_pi * i })));
		EXPECT_FALSE((idk::rad{ idk::pi }.abs_comp(idk::rad{ idk::pi + 1.f + idk::two_pi * i })));
	}
}