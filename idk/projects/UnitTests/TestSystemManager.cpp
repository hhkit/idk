#include "pch.h"
#include <idk.h>
#include <core/SystemManager.h>
#include <app/Application.h>

class TestApplication : 
	public idk::Application
{
public:
	int i = 0;
	void Init() override { i++;  std::cout << "application init\n"; }
	void PollEvents() override { ++i; }
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