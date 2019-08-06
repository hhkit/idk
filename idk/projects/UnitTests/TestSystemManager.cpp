#include "pch.h"
#include <idk.h>
#include <core/Core.h>
#include <app/Application.h>
class TestApplication : 
	public idk::Application
{
public:
	int i = 0;
	void Init() override { i++;  std::cout << "application init\n"; }
	void PollEvents() override { ++i; }
	idk::vec2 GetMouseScreenPos() override { return idk::vec2{}; };
	idk::vec2 GetMouseScreenDel() override { return idk::vec2{}; };
	bool GetKeyDown(idk::Key) override { return false; };
	bool GetKey(idk::Key) override { return false; };
	bool GetKeyUp(idk::Key) override { return false; };

	// windows
	bool SetFullscreen(bool ) override { return false; };
	bool SetScreenSize(idk::ivec2 ) override { return false; };
	void SwapBuffers() override { if (i == 50) idk::Core::Shutdown(); std::cout << "swap buffer" << i << '\n'; };

	void Shutdown() override { i++;  std::cout << "application shutdown\n"; }
};

TEST(System, TestSystemManager)
{
	using namespace idk;

	auto app = std::make_shared<TestApplication>();
	idk::SystemManager sysman(app, std::shared_ptr<IEditor>());

	sysman.GetSystem<Application>();

	sysman.InitSystems();
	EXPECT_EQ(app-> i, 1);
	sysman.ShutdownSystems();
	EXPECT_EQ(app->i, 2);
}

TEST(System, TestCore)
{
	using namespace idk;

	auto core = idk::Core::MakeCore<TestApplication>();

	//Core::GetSystem<Application>().Init();
	auto scene = GameState::GetGameState().ActivateScene(0);
	scene->CreateGameObject();
	scene->CreateGameObject();
	scene->CreateGameObject();
	core.Run();
}