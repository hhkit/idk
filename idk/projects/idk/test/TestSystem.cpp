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
			auto tfm = elem.GetGameObject()->Transform();
			if (app_sys.GetKey(Key::A)) tfm->position += vec3{ -0.016, 0.0, 0.0 };
			if (app_sys.GetKey(Key::D)) tfm->position += vec3{ +0.016, 0.0, 0.0 };
			if (app_sys.GetKey(Key::S)) tfm->position += vec3{ 0, -0.016, 0.0 };
			if (app_sys.GetKey(Key::W)) tfm->position += vec3{ 0, +0.016, 0.0 };

			if (app_sys.GetKey(Key::Q)) tfm->rotation = (quat{ vec3{0,1,0}, deg{90} *Core::GetDT().count() } *tfm->rotation).normalize();
			if (app_sys.GetKey(Key::E)) tfm->rotation = (quat{ vec3{0,1,0}, deg{-90} *Core::GetDT().count() } *tfm->rotation).normalize();
		}
	}

	void TestSystem::Shutdown()
	{
		std::cout << "Shutdown System\n";
	}
}
