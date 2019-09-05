#include "stdafx.h"

#include <iostream>
#include "TestSystem.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <test/TestComponent.h>
#include <app/Application.h>
#include <gfx/GraphicsSystem.h>
#include <gfx/Camera.h>
#include <core/GameState.h>

namespace idk
{
	void TestSystem::Init()
	{
		std::cout << "Init System\n";
	}

	void TestSystem::TestSpan(span<TestComponent> comps)
	{

		//auto& app_sys = Core::GetSystem<Application>();
		////assert(currCamera);

		//Handle<Camera> currCamera = main_camera.current_camera;
		//auto tfm = currCamera->GetGameObject()->GetComponent<Transform>();

		////Please ignore all of this first
		//if (app_sys.GetKey(Key::A)) tfm->position += vec3{ +0.016, 0.0, 0.0 };
		//if (app_sys.GetKey(Key::D)) tfm->position += vec3{ -0.016, 0.0, 0.0 };
		////if (app_sys.GetKey(Key::S)) tfm->position += vec3{ 0, -0.016, 0.0 };
		////if (app_sys.GetKey(Key::W)) tfm->position += vec3{ 0, +0.016, 0.0 };
		//if (app_sys.GetKey(Key::S)) tfm->position += vec3{ 0, 0.0, -0.016 };
		//if (app_sys.GetKey(Key::W)) tfm->position += vec3{ 0, 0.0, +0.016 };
		//if (app_sys.GetKey(Key::F)) tfm->position += vec3{ 0, -0.016, 0.0 };
		//if (app_sys.GetKey(Key::G)) tfm->position += vec3{ 0, +0.016, 0.0 };

		////Please ignore all of this first
		//if (app_sys.GetKey(Key::Q)) tfm->rotation = (quat{ vec3{0,1,0}, deg{90} *Core::GetDT().count() } *tfm->rotation).normalize();
		//if (app_sys.GetKey(Key::E)) tfm->rotation = (quat{ vec3{0,1,0}, deg{-90} *Core::GetDT().count() } *tfm->rotation).normalize();
		//if (app_sys.GetKey(Key::Z)) tfm->rotation = (quat{ vec3{1,0,0}, deg{90} *Core::GetDT().count() } *tfm->rotation).normalize();
		//if (app_sys.GetKey(Key::C)) tfm->rotation = (quat{ vec3{1,0,0}, deg{-90} *Core::GetDT().count() } *tfm->rotation).normalize();


		//if (app_sys.GetKey(Key::P))
		//{
		//	auto first = GameState::GetGameState().GetObjectsOfType<GameObject>()[0].GetHandle();
		////	currCamera->SetTarget(first->GetComponent<Transform>()->GlobalPosition());
		////	currCamera->Focus();
		//	main_camera.SetTarget(first->GetComponent<Transform>());
		//	//currCamera->LookAt(first->Transform()->GlobalPosition());
		//	main_camera.Focus();
		//}

		////std::cout << std::boolalpha << app_sys.IsMouseDragging() << std::endl;
		//if (app_sys.GetKey(Key::RButton) && app_sys.IsMouseDragging(Key::RButton))
		//{

		//	//This is not referencing the sc
		//	vec2 newPos = app_sys.GetMouseScreenPos();
		//	//ivec2 newPos2 = app_sys.GetMousePixelPos();

		//	vec2 anotherPos = vec2{newPos.x,newPos.y};

		//	main_camera.StartRotatingArcballCamera();
		//	main_camera.RotateArcBallCamera(anotherPos);

		//	//currCamera->RotateArcBallCamera(newPos);
		//	//tfm->rotation = (quat{ vec3{newPos.x,newPos.y,0}, deg{-90} *Core::GetDT().count() } *tfm->rotation).normalize();
		//	std::cout << newPos.x << "||" << newPos.y << std::endl;
		//}
		//else
		//{
		//	main_camera.StopRotatingArcBallCamera();
		//}
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
