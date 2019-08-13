#include "pch.h"
#include <core/GameState.h>
#include <core/GameObject.h>
#include <common/Transform.h>

TEST(GameState, TestGameState)
{
	using namespace idk;
	GameState gs;
	gs.ActivateScene(0);
	auto h = gs.CreateObject<GameObject>(0);
	auto hTransform = h->AddComponent<Transform>();
	EXPECT_TRUE(gs.ValidateHandle(h));
	EXPECT_TRUE(gs.GetObject(h));
	gs.DestroyObject(h);
	(*hTransform).position = vec3{0.f, 0.f, 5.f};
}

TEST(GameState, TestHandles)
{
	using namespace idk;
	GameState gs;
	gs.ActivateScene(0);
	auto h = gs.CreateObject<GameObject>(0);
	auto hTransform = h->AddComponent<Transform>();
	auto gh = GenericHandle{ h };
	EXPECT_TRUE(h);
	EXPECT_TRUE(&*h);
	gs.DestroyObject(h);
	(*hTransform).position = vec3{ 0.f, 0.f, 5.f };
	hTransform->rotation = quat{ vec3{0.f, 1.f, 0.f}, rad{pi} };
	EXPECT_TRUE(&*h);
	EXPECT_TRUE(&*hTransform);
	auto tfm = hTransform->GetMatrix();
	gs.DestroyQueue();
	EXPECT_FALSE(&*h);
	EXPECT_FALSE(&*hTransform);
}

TEST(GameState, TestScene)
{
	using namespace idk;
	GameState gs;
	auto scene0 = gs.ActivateScene(0);
	auto scene1 = gs.ActivateScene(1);
	EXPECT_TRUE(scene0);
	EXPECT_TRUE(scene1);
	
	const auto create_in_scene0 = 10;
	for (int i = 0; i < create_in_scene0; ++i)
		scene0->CreateGameObject();

	const auto create_in_scene1 = 5;
	for (int i = 0; i < create_in_scene1; ++i)
		scene1->CreateGameObject();

	int count = 0;
	for (auto& elem : *scene0)
		++count;

	EXPECT_EQ(count, create_in_scene0);

	count = 0;
	for (auto& elem : Scene{ 1 })
		++count;
	EXPECT_EQ(count, create_in_scene1);

	count = 0;
	for (auto& elem : Scene{ 2 })
		++count;
	EXPECT_EQ(count, 0);
}