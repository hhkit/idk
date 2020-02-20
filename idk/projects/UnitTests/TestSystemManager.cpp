#include "pch.h"
#include <idk.h>
#include <core/Core.inl>
#include <core/GameObject.inl>
#include <app/Application.h>
#include <editor/IEditor.h>
#include <scene/SceneManager.h>
#include <scene/SceneGraph.inl>
#include <res/ResourceManager.inl>
#include <common/Transform.h>
#include <res/ResourceHandle.inl>

/*
TEST(System, TestSystemManager)
{
	using namespace idk;

	idk::SystemManager sysman;
	auto app = &sysman.AddSystem<TestApplication>();
	sysman.GetSystem<Application>();
	sysman.GetSystem<TestApplication>();
	sysman.GetSystem<IEditor>();

	sysman.InitSystems();
	EXPECT_EQ(app-> i, 1);
	sysman.ShutdownSystems();
	EXPECT_EQ(app->i, 2);
}*/

TEST(System, TestCore)
{
	using namespace idk;

    INIT_CORE();
	
	auto curr_scene = core.GetSystem<SceneManager>().GetActiveScene();
	auto obj1 = curr_scene->CreateGameObject();
	curr_scene->CreateGameObject();
	curr_scene->CreateGameObject();

	curr_scene->CreateGameObject()->Transform()->parent = obj1;
	curr_scene->CreateGameObject();

	core.GetSystem<SceneManager>().BuildSceneGraph(GameState::GetGameState().GetObjectsOfType<const GameObject>());
	auto sg = core.GetSystem<SceneManager>().FetchSceneGraph();

	int indent = 0;
	sg.Visit([&indent](const auto& handle, int depth) {
		indent += depth;
		for (int i = 0; i < indent; ++i)
			std::cout << ' ';
		std::cout << handle.id << '\n';
	});
	// IT WORKSSSSSSSSS
	//core.Run();
}