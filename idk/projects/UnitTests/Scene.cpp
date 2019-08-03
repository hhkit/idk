#include "pch.h"
#include <core/Scene.h>
#include <core/GameObject.h>

TEST(Scene, SceneSpecifiedType)
{
	using namespace idk;

	ObjectPool<GameObject> entities{ 5 };
	auto h = entities.emplace();

	Scene s;
	EXPECT_FALSE(s.CheckHandle(h));
}