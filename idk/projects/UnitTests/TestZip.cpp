#include "pch.h"
#include <idk.h>
#include <ds/zip.h>

TEST(Zip, ZipTest)
{
	idk::vec2 v{ 1.f, 2.f };
	idk::vec2 v2{ 1.f, 2.f };
	for (auto& [a, b] : idk::zip(v, v2))
	{
		EXPECT_EQ(a, b);
	}

	idk::vec3 v3{ 1.f, 2.f, 3.f };
	idk::vec3 v4{ 2.f, 3.f, 4.f };
	auto zipped = idk::zip(v3, v4, idk::vec4{ 2.f, 6.f, 12.f, 24.f });
	for (auto& [l, r, res] : zipped)
	{
		EXPECT_EQ(l * r, res);
	}
}