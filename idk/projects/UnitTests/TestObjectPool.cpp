#include "pch.h"
#include "../IDK/idk.h"
#include "../IDK/core/ObjectPool.h"
#include "../IDK/core/Entity.h"
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
}

TEST(ObjectPool, TestObjectPooling)
{
	using namespace idk;

	ObjectPool<Entity> entities{5};
	auto h = entities.emplace();
	auto h2 = entities.emplace();
	for (int i = 0; i < 10; ++i)
	{
		entities.emplace();
	}
	*entities.at(h) = Entity{};
	entities.remove(h);
	EXPECT_EQ(entities.at(h), nullptr);
	EXPECT_NE(entities.at(h2), nullptr);
	entities.emplace();
}