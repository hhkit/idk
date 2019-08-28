#include "pch.h"
#include <ds/dual_set.h>

TEST(DualSet, TestInsert)
{
	using namespace idk;

	dual_set<string, int> potato;
	EXPECT_TRUE(potato.insert(std::pair<std::string, int>("pot", 5)));
	EXPECT_FALSE(potato.insert(std::pair<std::string, int>("pot", 5)));
	EXPECT_FALSE(potato.insert(std::pair<std::string, int>("pot", 4)));
	EXPECT_FALSE(potato.insert(std::pair<std::string, int>("pota", 5)));
	EXPECT_TRUE(potato.insert(std::pair<std::string, int>("pota", 4)));
}

TEST(DualSet, TestEmplace)
{
	using namespace idk;

	dual_set<string, int> potato;
	EXPECT_TRUE(potato.emplace("pot", 5));
	EXPECT_FALSE(potato.emplace("pot", 5));
	EXPECT_FALSE(potato.emplace("pot", 4));
	EXPECT_FALSE(potato.emplace("pota", 5));
	EXPECT_TRUE (potato.emplace("pota", 4));
}

TEST(DualSet, TestFind)
{
	using namespace idk;

	dual_set<string, int> potato;
	potato.emplace("haha", 5);
	potato.emplace("hahah", 4);
	potato.emplace("hahaha", 3);

	EXPECT_EQ(potato.size(), 3);

	EXPECT_TRUE(potato.find_first("haha") != potato.end());
	EXPECT_TRUE(potato.find_first("hahah") != potato.end());
	EXPECT_TRUE(potato.find_first("hahaha") != potato.end());
	EXPECT_FALSE(potato.find_first("hahahaha") != potato.end());

	EXPECT_TRUE(potato.find_second(5) != potato.end());
	EXPECT_TRUE(potato.find_second(4) != potato.end());
	EXPECT_TRUE(potato.find_second(3) != potato.end());
	EXPECT_FALSE(potato.find_second(2) != potato.end());
	EXPECT_FALSE(potato.find_second(1) != potato.end());

	EXPECT_TRUE(potato.find("haha") != potato.end());
	EXPECT_TRUE(potato.find("hahah") != potato.end());
	EXPECT_TRUE(potato.find("hahaha") != potato.end());
	EXPECT_FALSE(potato.find("hahahaha") != potato.end());

	EXPECT_TRUE(potato.find(5) != potato.end());
	EXPECT_TRUE(potato.find(4) != potato.end());
	EXPECT_TRUE(potato.find(3) != potato.end());
	EXPECT_FALSE(potato.find(2) != potato.end());
	EXPECT_FALSE(potato.find(1) != potato.end());

	
}