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