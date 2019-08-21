#include "pch.h" // gtest.h
#include <ds/small_string.h>

TEST(SmallString, TestSmallString)
{
	EXPECT_EQ(sizeof(idk::small_string<char>), 32);

	idk::small_string<char> str;
}