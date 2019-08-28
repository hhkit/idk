#include "stdafx.h"

#include <iostream>
#include "TestSystem.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <test/TestComponent.h>
#include <app/Application.h>

namespace idk
{
	void TestSystem::Init()
	{
		std::cout << "Init System\n";
	}

	void TestSystem::TestSpan(span<TestComponent> comps)
	{
		for (auto& elem : comps)
		{
			auto& app_sys = Core::GetSystem<Application>();

			if (app_sys.GetKey(Key::A)) elem.GetGameObject()->GetComponent<Transform>()->position += vec3{ -0.016, 0.0, 0.0 };
			if (app_sys.GetKey(Key::D)) elem.GetGameObject()->GetComponent<Transform>()->position += vec3{ +0.016, 0.0, 0.0 };
			if (app_sys.GetKey(Key::S)) elem.GetGameObject()->GetComponent<Transform>()->position += vec3{ 0, -0.016, 0.0 };
			if (app_sys.GetKey(Key::W)) elem.GetGameObject()->GetComponent<Transform>()->position += vec3{ 0, +0.016, 0.0 };
		}
	}

	void TestSystem::Shutdown()
	{
		std::cout << "Shutdown System\n";
	}
}
