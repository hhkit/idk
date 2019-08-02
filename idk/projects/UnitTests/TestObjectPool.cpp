#include "pch.h"
#include "../IDK/idk.h"
#include "../IDK/core/ObjectPool.h"
#include <iostream>

TEST(ObjectPool, TestIndexInTuple)
{
	EXPECT_EQ(
		(
			idk::detail::index_in_tuple<
			float,
			std::tuple<float, float, int, short, short, int, char>
			>::value
			)
		, 0);
	EXPECT_EQ(
		(
			idk::detail::index_in_tuple<
				int, 
				std::tuple<float, float, int, short, short, int, char>
			>::value
		)
		, 2);
	EXPECT_EQ(
		(
			idk::detail::index_in_tuple<
			double,
			std::tuple<float, float, int, short, short>
			>::value
			)
		, 5);

	idk::ObjectHandle<int>{5, 2};
	idk::ObjectPool<int> int_pool;
	auto h = int_pool.emplace();

	*int_pool.at(h) = 5;
	EXPECT_EQ(
		*int_pool.at(h), 5
	);
}