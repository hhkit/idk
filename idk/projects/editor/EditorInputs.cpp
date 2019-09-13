
//Required
#include "pch.h"
#include "EditorInputs.h"

//STL
#include <iostream>

//Main Stuffs//
//Core
#include <core/GameObject.h>
#include <core/GameState.h>

//App
#include <app/Application.h>

//Commons
#include <common/Transform.h>

//Graphics
#include <gfx/GraphicsSystem.h>
#include <gfx/Camera.h>

namespace idk
{
	void EditorInputs::Update()
	{
		auto& app_sys = Core::GetSystem<Application>();
		//assert(currCamera);

		Handle<Camera> currCamera = main_camera.current_camera;
		auto tfm = currCamera->GetGameObject()->GetComponent<Transform>();

		//Please ignore all of this first
		constexpr auto cam_vel = 1.f;
		if (app_sys.GetKey(Key::A))       tfm->position += - cam_vel * Core::GetRealDT().count() * tfm->Right();
		if (app_sys.GetKey(Key::D))       tfm->position += + cam_vel * Core::GetRealDT().count() * tfm->Right();
		//if (app_sys.GetKey(Key::S)) tfm->position += vec3{ 0, -0.016, 0.0 };
		//if (app_sys.GetKey(Key::W)) tfm->position += vec3{ 0, +0.016, 0.0 };
		if (app_sys.GetKey(Key::S))       tfm->position += +cam_vel * Core::GetRealDT().count() * tfm->Forward();
		if (app_sys.GetKey(Key::W))       tfm->position += -cam_vel * Core::GetRealDT().count() * tfm->Forward();
		if (app_sys.GetKey(Key::Control)) tfm->position += -cam_vel * Core::GetRealDT().count() * vec3{0, 1, 0};
		if (app_sys.GetKey(Key::Space))   tfm->position += +cam_vel * Core::GetRealDT().count() * vec3{0, 1, 0};

		//Please ignore all of this first
		if (app_sys.GetKey(Key::Q)) tfm->rotation = (quat{ vec3{0,1,0}, deg{90} *Core::GetDT().count() } *tfm->rotation).normalize();
		if (app_sys.GetKey(Key::E)) tfm->rotation = (quat{ vec3{0,1,0}, deg{-90} *Core::GetDT().count() } *tfm->rotation).normalize();
		if (app_sys.GetKey(Key::Z)) tfm->rotation = (quat{ vec3{1,0,0}, deg{90} *Core::GetDT().count() } *tfm->rotation).normalize();
		if (app_sys.GetKey(Key::C)) tfm->rotation = (quat{ vec3{1,0,0}, deg{-90} *Core::GetDT().count() } *tfm->rotation).normalize();
		if (app_sys.GetKey(Key::V)) tfm->rotation = (quat{ vec3{0,0,1}, deg{90} *Core::GetDT().count() } *tfm->rotation).normalize();
		if (app_sys.GetKey(Key::B)) tfm->rotation = (quat{ vec3{0,0,1}, deg{-90} *Core::GetDT().count() } *tfm->rotation).normalize();


		if (app_sys.GetKeyUp(Key::P))
		{
			auto first = GameState::GetGameState().GetObjectsOfType<GameObject>()[_curr_cycle].GetHandle();

			if (first->HasComponent<Camera>())
			{
				_curr_cycle = (_curr_cycle + 1) % GameState::GetGameState().GetObjectsOfType<GameObject>().size();
				first = GameState::GetGameState().GetObjectsOfType<GameObject>()[_curr_cycle].GetHandle();
			}
			_curr_cycle = (_curr_cycle+1) % GameState::GetGameState().GetObjectsOfType<GameObject>().size();
			//	currCamera->SetTarget(first->GetComponent<Transform>()->GlobalPosition());
			//	currCamera->Focus();
			main_camera.SetTarget(first->GetComponent<Transform>());
			//currCamera->LookAt(first->Transform()->GlobalPosition());
			//main_camera.LookAt();

			main_camera.Focus();
		}

		//std::cout << std::boolalpha << app_sys.IsMouseDragging() << std::endl;
		if (app_sys.GetKey(Key::RButton))
		{

			//This is not referencing the sc
			vec2 newPos = app_sys.GetMouseScreenPos();
			//ivec2 newPos2 = app_sys.GetMousePixelPos();

			vec2 anotherPos = vec2{ newPos.x,-newPos.y };

			if(!main_camera._rotating)
				main_camera.StartRotatingCamera(anotherPos);
			main_camera.RotateCamera(anotherPos);

			//currCamera->RotateArcBallCamera(newPos);
			//tfm->rotation = (quat{ vec3{newPos.x,newPos.y,0}, deg{-90} *Core::GetDT().count() } *tfm->rotation).normalize();
			//std::cout << newPos.x << "||" << newPos.y << std::endl;
		}
		else
		{
			if (main_camera._rotating)
				main_camera.StopRotatingCamera();
		}
		//std::cout << app_sys.GetKey(Key::MButton) << "\n";
		if (app_sys.GetKey(Key::MButton))
		{
			vec2 newPos = app_sys.GetMouseScreenPos();
			//ivec2 newPos2 = app_sys.GetMousePixelPos();

			vec2 anotherPos = vec2{ newPos.x,-newPos.y };

			if (!main_camera._panning)
				main_camera.StartPanningCamera(anotherPos);
			main_camera.PanCamera(anotherPos);
		}
		else
		{
			if (main_camera._panning)
				main_camera.StopPanningCamera();
		}
	}
};
