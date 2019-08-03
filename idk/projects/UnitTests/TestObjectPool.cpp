#include "pch.h"
#include "../IDK/idk.h"
#include "../IDK/core/ObjectPool.h"
#include "../IDK/core/GameObject.h"
#include "../IDK/core/Component.h"
#include <core/Scene.h>
#include <iostream>

class TestComponent : public idk::Component<TestComponent>
{

};


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

	ObjectPool<GameObject> entities{5};
	auto h = entities.emplace(nullptr);
	auto h2 = entities.emplace(nullptr);
	for (int i = 0; i < 10; ++i)
	{
		entities.emplace(nullptr);
	}
	*entities.at(h) = GameObject{nullptr};
	entities.remove(h);
	EXPECT_EQ(entities.at(h), nullptr);
	EXPECT_NE(entities.at(h2), nullptr);
	entities.emplace(nullptr);

	TestComponent t;
	t.GetHandle();


}