#include "pch.h"
#include <core/Core.inl>
#include <core/GameState.inl>
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <phys/Collider.h>
#include <scene/SceneFactory.h>
#include <scene/SceneManager.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>

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
	EXPECT_TRUE(gs.ActivateScene(0));
	auto h = gs.CreateObject<GameObject>(0);
	auto hTransform = h->AddComponent<Transform>();
	h->AddComponent<Collider>();
	h->AddComponent<Collider>();
	auto removeme = h->AddComponent<Collider>();
	EXPECT_EQ(h->GetComponents<Collider>().size(), 3);
	h->RemoveComponent(removeme);
	gs.DestroyQueue();
	EXPECT_EQ(h->GetComponents<Collider>().size(), 2);
	EXPECT_EQ(h->GetComponents<MeshRenderer>().size(), 0);

	auto gh = GenericHandle{ h };
	EXPECT_TRUE(h);
	EXPECT_TRUE(&*h);
	gs.DestroyObject(h);
	(*hTransform).position = vec3{ 0.f, 0.f, 5.f };
	hTransform->rotation = quat{ vec3{0.f, 1.f, 0.f}, rad{pi} };
	EXPECT_TRUE(&*h);
	EXPECT_TRUE(&*hTransform);
	auto tfm = hTransform->LocalMatrix();
	gs.DestroyQueue();
	EXPECT_THROW(*h, NullHandleException);
    EXPECT_THROW(*hTransform, NullHandleException);

	gs.CreateObject(h);
	EXPECT_TRUE(h);
}

TEST(GameState, TestScene)
{
    using namespace idk;
    INIT_CORE();
    auto scene0 = Core::GetResourceManager().LoaderEmplaceResource<Scene>(0); scene0->LoadFromResourcePath();
	auto scene1 = Core::GetResourceManager().LoaderEmplaceResource<Scene>(1); scene1->LoadFromResourcePath();
	auto scene2 = Core::GetResourceManager().LoaderEmplaceResource<Scene>(2); scene2->LoadFromResourcePath();
	EXPECT_TRUE(scene0);
	EXPECT_TRUE(scene1);
	EXPECT_TRUE(scene2);
	
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
	for (auto& elem : *scene1)
		++count;
	EXPECT_EQ(count, create_in_scene1);

	count = 0;
	for (auto& elem : *scene2)
		++count;
	EXPECT_EQ(count, 0);
}