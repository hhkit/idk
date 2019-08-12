#include "pch.h"
#include <util/string_hash.h>

TEST(String, StringHash)
{
	using namespace idk;


	EXPECT_EQ(string_hash("yolo"), string_hash("yolo"));

	constexpr auto yolo_hash = string_hash("yolo");
	EXPECT_EQ(string_hash("yolo"), yolo_hash);

	EXPECT_NE(string_hash("yolo"), string_hash("kje"));
}