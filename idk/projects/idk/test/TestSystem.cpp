#include "stdafx.h"

#include <iostream>
#include "TestSystem.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <test/TestComponent.h>
#include <test/TestResourceFactory.h>
#include <app/Application.h>
#include <app/GamepadSystem.h>
#include <phys/RigidBody.h>
#include <gfx/GraphicsSystem.h>
#include <gfx/Camera.h>
#include <core/GameState.h>
#include <proj/ProjectManager.h>
#include <gfx/DebugRenderer.h>
#include <PauseConfigurations.h>
#include <file/FileSystem.h>
#include <gfx/ShaderGraph.h>
#include <parallel/ThreadPool.h>
namespace idk
{
	void TestSystem::Init()
	{
		std::cout << "Init System\n";
		Core::GetResourceManager().RegisterFactory<TestResourceFactory>();

		//GameState::GetGameState().OnObjectCreate<GameObject>() += 
		//	[](Handle<GameObject> h)
		//	{
		//		std::cout << "created: " << h.id << '\n';
		//	};
		//GameState::GetGameState().OnObjectDestroy<GameObject>() +=
		//	[](Handle<GameObject> h)
		//{
		//	std::cout << "destroyed: " << h.id << '\n';
		//};
	}

	void TestSystem::TestSpan(span<TestComponent> comps)
	{
		static float t = 0.f;
		t += Core::GetDT().count();
		auto& app_sys = Core::GetSystem<Application>();
		auto& gamepad = Core::GetSystem<GamepadSystem>();
		
		static bool fire = false;
		if (app_sys.GetKey(Key::I) )
		{
			/*
			mt::circular_buffer_lf<int, 4> poop;

			int i = 0;
			for (int iter = 0; iter < 2; ++iter)
			{
				for (int j = 0; j < 8; ++j, ++i)
					poop.emplace_back(i);
				while (auto val = poop.pop_front())
					std::cout << *val << std::endl;
			}
			/*/
			const int jobs = rand() % 70 + 10;
			std::cout << "jobs: " << jobs << "\n";
			vector<mt::ThreadPool::Future<void>> futures(jobs);
			for (int i =0; i < jobs; ++i)
				futures[i] = Core::GetThreadPool().Post([i]()
				{
					std::cout << "hello from " + std::to_string(mt::thread_id()) + " with index " + std::to_string(i) + "\n";
				});
			for (auto& elem : futures)
				elem.get();
			std::cout << "all printed\n";
			fire = true;
			//*/
		}
		
		for (auto& elem : comps)
		{
			if (app_sys.GetKey(Key::J)) elem.GetGameObject()->Transform()->position += vec3{ +0.016, 0.0, 0.0 };
			if (app_sys.GetKey(Key::L)) elem.GetGameObject()->Transform()->position += vec3{ -0.016, 0.0, 0.0 };
			if (app_sys.GetKey(Key::K)) elem.GetGameObject()->Transform()->position += vec3{ 0, 0.0, -0.016 };
			if (app_sys.GetKey(Key::I)) elem.GetGameObject()->Transform()->position += vec3{ 0, 0.0, +0.016 };

            if (gamepad.GetAxis(0, GamepadAxis::LeftStickX)) std::cout <<  "LeftStickX" << gamepad.GetAxis(0, GamepadAxis::LeftStickX) << std::endl;
            if (gamepad.GetAxis(0, GamepadAxis::LeftStickY)) std::cout <<  "LeftStickY" << gamepad.GetAxis(0, GamepadAxis::LeftStickY) << std::endl;
            if (gamepad.GetAxis(0, GamepadAxis::LeftTrigger)) std::cout << "LeftTrigger" <<  gamepad.GetAxis(0, GamepadAxis::LeftTrigger) << std::endl;
            if (gamepad.GetButtonDown(0, GamepadButton::LeftShoulder)) std::cout << "LeftShoulder" << gamepad.GetButtonDown(0, GamepadButton::LeftShoulder) << std::endl;
            if (gamepad.GetButtonDown(0, GamepadButton::DPadLeft)) std::cout << "DPadLeft" << gamepad.GetButtonDown(0, GamepadButton::DPadLeft) << std::endl;
            if (gamepad.GetButtonDown(0, GamepadButton::A)) std::cout << "A" << gamepad.GetButtonDown(0, GamepadButton::A) << std::endl;
            if (gamepad.GetButtonDown(0, GamepadButton::Start)) std::cout << "Start" << gamepad.GetButtonDown(0, GamepadButton::Start) << std::endl;

            gamepad.SetRumble(0, gamepad.GetAxis(0, GamepadAxis::LeftStickY), gamepad.GetAxis(0, GamepadAxis::RightStickY));

			//auto rb = elem.GetGameObject()->GetComponent<RigidBody>();
			//if (rb)
			//	rb->AddForce(vec3{ 1, 0, 0 } * sin(rad{t / 0.01f}));
		}
		{
			bool recompile_graph = false;
			for (auto& file : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN))
			{
				switch (string_hash(file.GetExtension()))
				{
				case string_hash(".glsl"):
					Core::GetResourceManager().Load<ShaderSnippet>(file); //Update the shader snippet
				case string_hash(".tmpt"):
					recompile_graph = true;
					break;
				default:
					break;
				}
			}
			if (recompile_graph) 
			{
				for (auto& elem : Core::GetResourceManager().GetAll<shadergraph::Graph>())
					elem->Compile();
				Core::GetSystem<GraphicsSystem>().LoadShaders();
			}
		}

		if (app_sys.GetKey(Key::Control) && app_sys.GetKeyDown(Key::S))
			Core::GetSystem<ProjectManager>().SaveProject();
	}

	void TestSystem::Shutdown()
	{
		std::cout << "Shutdown System\n";
	}
}
