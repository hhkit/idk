#include "stdafx.h"

#include <iostream>
#include "TestSystem.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <test/TestComponent.h>
#include <app/Application.h>
#include <gfx/GraphicsSystem.h>
#include <gfx/Camera.h>

namespace idk
{
	void TestSystem::Init()
	{
		std::cout << "Init System\n";
	}

	void TestSystem::TestSpan(span<TestComponent> comps)
	{
		Handle<Camera> currCamera = Core::GetSystem<GraphicsSystem>().CurrentCamera();

		auto& app_sys = Core::GetSystem<Application>();
		//assert(currCamera);
		auto tfm = currCamera->GetGameObject()->GetComponent<Transform>();

		//Please ignore all of this first
		if (app_sys.GetKey(Key::A)) tfm->position += vec3{ +0.016, 0.0, 0.0 };
		if (app_sys.GetKey(Key::D)) tfm->position += vec3{ -0.016, 0.0, 0.0 };
		if (app_sys.GetKey(Key::S)) tfm->position += vec3{ 0, -0.016, 0.0 };
		if (app_sys.GetKey(Key::W)) tfm->position += vec3{ 0, +0.016, 0.0 };

		//Please ignore all of this first
		if (app_sys.GetKey(Key::Q)) tfm->rotation = (quat{ vec3{0,1,0}, deg{90} *Core::GetDT().count() } *tfm->rotation).normalize();
		if (app_sys.GetKey(Key::E)) tfm->rotation = (quat{ vec3{0,1,0}, deg{-90} *Core::GetDT().count() } *tfm->rotation).normalize();
		if (app_sys.GetKey(Key::Z)) tfm->rotation = (quat{ vec3{1,0,0}, deg{90} *Core::GetDT().count() } *tfm->rotation).normalize();
		if (app_sys.GetKey(Key::C)) tfm->rotation = (quat{ vec3{1,0,0}, deg{-90} *Core::GetDT().count() } *tfm->rotation).normalize();

		//std::cout << std::boolalpha << app_sys.IsMouseDragging() << std::endl;
		if (app_sys.GetKey(Key::RButton) && app_sys.IsMouseDragging())
		{
			vec2 newPos = app_sys.GetMouseScreenPos();
			//ivec2 newPos2 = app_sys.GetMousePixelPos();

			vec2 anotherPos = vec2{newPos.x,newPos.y};

			currCamera->StartRotatingArcballCamera();
			currCamera->RotateArcBallCamera(anotherPos);

			//currCamera->RotateArcBallCamera(newPos);
			//tfm->rotation = (quat{ vec3{newPos.x,newPos.y,0}, deg{-90} *Core::GetDT().count() } *tfm->rotation).normalize();
			std::cout << newPos.x << "||" << newPos.y << std::endl;
		}
		else
		{
			currCamera->StopRotatingArcBallCamera();
		}
		//currCamera->ViewMatrix();

		//currCamera->ProjectionMatrix();

		/*
		for (auto& elem : comps)
		{
			//auto& app_sys = Core::GetSystem<Application>();
			if (elem.GetGameObject()->HasComponent<Camera>())
				continue;

			auto tfm = elem.GetGameObject()->Transform();
		}
		*/
	}

	void TestSystem::Shutdown()
	{
		std::cout << "Shutdown System\n";
	}
}
