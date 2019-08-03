#include "pch.h"
#include <core/Scene.h>
#include <core/GameObject.h>
#include <common/Transform.h>

TEST(Core, SceneSpecifiedType)
{
	using namespace idk;

	Scene s;

	auto h = s.CreateGameObject();
	auto go = s.GetObject(h);
	auto th = go->AddComponent<Transform>();
	go->RemoveComponent(th);
	EXPECT_TRUE(s.Validate(h));
	EXPECT_FALSE(s.Validate(th));
}