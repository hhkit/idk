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

TEST(ObjectPool, TestObjectDefrag)
{
	using namespace idk;
	pool<int> p;

	for (int i = 0; i < 1000; ++i)
		p.emplace_back();

	for (int i = 0; i < 1000; ++i)
		p.pop_back();

	ObjectPool<class GameObject> op;
	op.ActivateScene(0);

	auto obj1 = op.Create(0);
	auto obj2 = op.Create(0);
	auto obj3 = op.Create(0);
	auto obj4 = op.Create(0);
	auto obj5 = op.Create(0);

	auto print_all = [&]()
	{
		std::cout << "handles: ";
		std::cout << obj1.id << ' ';
		std::cout << obj2.id << ' ';
		std::cout << obj3.id << ' ';
		std::cout << obj4.id << ' ';
		std::cout << obj5.id << '\n';
		std::cout << "objs: ";
		std::cout << op.Get(obj1)->GetHandle().id << ' ';
		std::cout << op.Get(obj2)->GetHandle().id << ' ';
		std::cout << op.Get(obj3)->GetHandle().id << ' ';
		std::cout << op.Get(obj4)->GetHandle().id << ' ';
		std::cout << op.Get(obj5)->GetHandle().id << '\n';
	};

	for (auto& elem : op.GetSpan())
		std::cout << elem.GetHandle().id << ' ';
	std::cout << '\n';
	print_all();
	// reverse
	std::cout << "swaps: " << op.Defrag([](const GameObject& lhs, const GameObject& rhs) {return lhs.GetHandle().id > rhs.GetHandle().id; }) << '\n';

	for (auto& elem : op.GetSpan())
		std::cout << elem.GetHandle().id << ' ';
	std::cout << '\n';
	print_all();

	std::cout << "swaps: " << op.Defrag([](const GameObject& lhs, const GameObject& rhs) {return lhs.GetHandle().id > rhs.GetHandle().id; }) << '\n';

	for (auto& elem : op.GetSpan())
		std::cout << elem.GetHandle().id << ' ';
	std::cout << '\n';
	print_all();

	std::cout << "swaps: " << op.Defrag([](const GameObject& lhs, const GameObject& rhs) {return lhs.GetHandle().id < rhs.GetHandle().id; }) << '\n';

	for (auto& elem : op.GetSpan())
		std::cout << elem.GetHandle().id << ' ';
	std::cout << '\n';
	print_all();
}