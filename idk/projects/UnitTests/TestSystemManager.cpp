#include "pch.h"
#include <idk.h>
#include <core/Core.h>
#include <app/Application.h>
#include <editor/IEditor.h>
#include <scene/SceneFactory.h>
#include <res/ResourceManager.h>
class TestApplication : 
	public idk::Application
{
public:
	int i = 0;
	void Init() override { i++;  std::cout << "application init\n"; }
	void PollEvents() override { ++i; }
	idk::vec2 GetMouseScreenPos() override { return idk::vec2{}; };
	idk::vec2 GetMouseScreenDel() override { return idk::vec2{}; };
	bool GetKeyDown(idk::CharKey) override { return false; };
	bool GetKey(idk::CharKey) override { return false; };
	bool GetKeyUp(idk::CharKey) override { return false; };
	bool GetKeyDown(idk::SysKey) override { return false; };
	bool GetKey(idk::SysKey) override { return false; };
	bool GetKeyUp(idk::SysKey) override { return false; };

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
	core.Run();
}