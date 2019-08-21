#include "pch.h" // gtest.h
#include <ds/small_string.h>

TEST(SmallString, TestSmallString)
{
	EXPECT_EQ(sizeof(idk::small_string<char>), 32);

	idk::small_string<char> str;
	EXPECT_EQ(str.size(), 0);
	EXPECT_STREQ(str.c_str(), "");

	str.push_back('a');
	EXPECT_EQ(str.size(), 1);
	EXPECT_EQ(str.capacity(), 31);
	EXPECT_STREQ(str.c_str(), "a");
}