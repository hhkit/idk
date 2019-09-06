#include "pch.h"
#include <idk.h>
#include <core/Core.h>
#include <core/GameObject.h>
#include <app/Application.h>
#include <editor/IEditor.h>
#include <scene/SceneManager.h>
#include <res/ResourceManager.h>
#include <common/Transform.h>
class TestApplication : 
	public idk::Application
{
public:
	int i = 0;
	void Init() override { i++;  std::cout << "application init\n"; }
	void PollEvents() override { ++i; }
	idk::vec2 GetMouseScreenPos() override { return idk::vec2{}; };
	idk::vec2 GetMouseScreenDel() override { return idk::vec2{}; };
    idk::ivec2 GetMousePixelPos() override { return idk::ivec2{}; };
    idk::ivec2 GetMousePixelDel() override { return idk::ivec2{}; };
    bool IsMouseDragging(idk::Key) override { return false; };
	bool GetKeyDown(idk::Key) override { return false; };
	bool GetKey(idk::Key) override { return false; };
	bool GetKeyUp(idk::Key) override { return false; };
	char GetChar() override { return 0; }
	idk::ivec2 GetScreenSize() override { return idk::ivec2{}; }

	// windows
	bool SetFullscreen(bool ) override { return false; };
	bool SetScreenSize(idk::ivec2 ) override { return false; };
	void SwapBuffers() override { if (i > 50) idk::Core::Shutdown(); std::cout << "swap buffer" << i << '\n'; };

	void Shutdown() override { i++;  std::cout << "application shutdown\n"; }
};
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

	auto core = Core{};
	core.AddSystem<TestApplication>();
	core.Setup();
	
	auto curr_scene = core.GetSystem<SceneManager>().GetActiveScene();
	auto obj1 = curr_scene->CreateGameObject();
	curr_scene->CreateGameObject();
	curr_scene->CreateGameObject();

	curr_scene->CreateGameObject()->Transform()->parent = obj1;
	curr_scene->CreateGameObject();

	core.GetSystem<SceneManager>().BuildSceneGraph(GameState::GetGameState().GetObjectsOfType<const GameObject>());
	auto& sg = core.GetSystem<SceneManager>().FetchSceneGraph();

	int indent = 0;
	sg.visit([&indent](const auto& handle, int depth) {
		indent += depth;
		for (int i = 0; i < indent; ++i)
			std::cout << ' ';
		std::cout << handle.id << '\n';
	});
	// IT WORKSSSSSSSSS
	//core.Run();
}