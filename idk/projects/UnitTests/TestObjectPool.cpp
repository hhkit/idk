#include "pch.h"
#include <idk.h>
#include <core/GameObject.h>
#include <core/Component.h>
#include <core/ObjectPool.h>
#include <reflect/reflect.h>
#include <iostream>


TEST(ObjectPool, TestIndexInTuple)
{
	EXPECT_EQ(
		(
			idk::index_in_tuple<
			float,
			std::tuple<float, float, int, short, short, int, char>
			>::value
			)
		, 0);
	EXPECT_EQ(
		(
			idk::index_in_tuple<
				int, 
				std::tuple<float, float, int, short, short, int, char>
			>::value
		)
		, 2);
	EXPECT_EQ(
		(
			idk::index_in_tuple<
			double,
			std::tuple<float, float, int, short, short>
			>::value
			)
		, 5);
}

TEST(ObjectPool, TestObjectPooling)
{
	using namespace idk;
	pool<int> p;

	for (int i = 0; i < 1000; ++i)
		p.emplace_back();

	for (int i = 0; i < 1000; ++i)
		p.pop_back();

	ObjectPool<class GameObject> op;
	op.ActivateScene(0);
	op.ActivateScene(2);
	op.Create(2);
	auto h = op.Create(0);
	auto h_fail = op.Create(1);
	auto h2 = op.Create(0);
		
	EXPECT_TRUE(op.Validate(h));
	EXPECT_TRUE(op.Validate(h2));
	EXPECT_FALSE(op.Validate(Handle<GameObject>{}));

	auto& go = *op.Get(h);
	op.Destroy(h);
	EXPECT_FALSE(op.Validate(h));
	EXPECT_TRUE(op.Validate(h2));

	op.DeactivateScene(0);
	GameState gs;

	GenericHandle generich;
	*generich;
}