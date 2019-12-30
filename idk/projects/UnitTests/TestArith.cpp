#include "pch.h"
#include <math/arith.inl>

TEST(Math, TestFloatFmod)
{
	using namespace idk;

	EXPECT_EQ(idk::fmod(0.f, 1.f), 0.f) << "Failed zero check";
	EXPECT_EQ(idk::fmod(5.f, 4.f), 1.f) << "Failed oversized integer mod";
	EXPECT_EQ(idk::fmod(4.f, 5.f), 4.f) << "Failed undersized integer mod";
	EXPECT_EQ(idk::fmod(-4.f, 5.f), 1.f) << "Failed negative integer mod";
	EXPECT_EQ(idk::fmod(-4.5f, 5.f), .5f) << "Failed negative floating numerator mod";
	EXPECT_EQ(idk::fmod(-4.75f, .5f), .25f) << "Failed negative floating denominator mod";
}