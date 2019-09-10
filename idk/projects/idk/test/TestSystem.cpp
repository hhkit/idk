#include "stdafx.h"

#include <iostream>
#include "TestSystem.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <test/TestComponent.h>
#include <test/TestResourceFactory.h>
#include <app/Application.h>
#include <gfx/GraphicsSystem.h>
#include <gfx/Camera.h>
#include <core/GameState.h>

namespace idk
{
	void TestSystem::Init()
	{
		std::cout << "Init System\n";
		Core::GetResourceManager().RegisterFactory<TestResourceFactory>();
	}

	void TestSystem::TestSpan(span<TestComponent> comps)
	{
		auto& app_sys = Core::GetSystem<Application>();

		for (auto& elem : comps)
		{
			if (app_sys.GetKey(Key::J)) elem.GetGameObject()->Transform()->position += vec3{ +0.016, 0.0, 0.0 };
			if (app_sys.GetKey(Key::L)) elem.GetGameObject()->Transform()->position += vec3{ -0.016, 0.0, 0.0 };
			if (app_sys.GetKey(Key::K)) elem.GetGameObject()->Transform()->position += vec3{ 0, 0.0, -0.016 };
			if (app_sys.GetKey(Key::I)) elem.GetGameObject()->Transform()->position += vec3{ 0, 0.0, +0.016 };
		}
	}

	void TestSystem::Shutdown()
	{
		std::cout << "Shutdown System\n";
	}
}
