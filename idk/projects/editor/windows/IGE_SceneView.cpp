//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_MainWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		30 AUG 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <editor/windows/IGE_SceneView.h>
#include <app/Application.h>
#include <common/Transform.h> //transform
#include <gfx/Camera.h> //camera
#include <core/GameObject.h>
#include <gfx/RenderTarget.h>
#include <iostream>
#include <math/euler_angles.h>
#include <gfx/GraphicsSystem.h>
#include <IDE.h>

namespace idk {

	IGE_SceneView::IGE_SceneView()
		:IGE_IWindow{ "SceneView",true,ImVec2{ 800,600 },ImVec2{ 0,50 } } {		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoCollapse
					 | ImGuiWindowFlags_NoScrollWithMouse
					 | ImGuiWindowFlags_NoScrollbar;

		//size_condition_flags = ImGuiCond_Always;
		//pos_condition_flags = ImGuiCond_Always;



	}

	void IGE_SceneView::BeginWindow()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		//ImGuiID dock_id = ImGui::GetID("IGEDOCKSPACE");
		//ImGui::DockBuilder

		
	}

	void IGE_SceneView::Update()
	{
		ImGui::PopStyleVar(3);

		ImVec2 imageSize{ GetScreenSize().x,GetScreenSize().y };
		//imageSize.y = (imageSize.x * (9 / 16));
		if (Core::GetSystem<GraphicsSystem>().GetAPI() != GraphicsAPI::Vulkan)
			ImGui::Image(RscHandle<RenderTarget>{}->GetMeta().textures[0]->ID(), imageSize, ImVec2(0,1),ImVec2(1,0));

		ImVec2 v = ImGui::GetWindowPos();

		ImVec2 currPos = ImGui::GetMousePosOnOpeningCurrentPopup();

		if (ImGui::IsMouseClicked(0)) {

			//Use this function to get
			//std::cout << "MousePosInWindow: " << GetMousePosInWindowNormalized().x << "," << GetMousePosInWindowNormalized().y << "\n";

			//Select gameobject here!
		}

		//Right Mouse control
		if (ImGui::IsMouseDown(1)) {
			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1)) { //Check if it is clicked here first!
				ImGui::SetWindowFocus();
				is_controlling_cam = true;
				//Core::GetSystem<IDE>()._interface->Inputs()->Update();
			}
		}
		else {
			is_controlling_cam = false;
		}

		if (is_controlling_cam) {
			UpdateMouseControl();
		}

	}

	void IGE_SceneView::SetTexture(void* textureToRender)
	{
		sceneTexture = textureToRender;
	}

	vec2 IGE_SceneView::GetScreenSize()
	{
		return vec2 { ImGui::GetWindowWidth(),ImGui::GetWindowHeight() - ImGui::GetFrameHeight() };
	}

	vec2 IGE_SceneView::GetMousePosInWindow()
	{
		ImVec2 v = ImGui::GetWindowPos();
		v.y += ImGui::GetFrameHeight();
		ImVec2 i = ImGui::GetMousePos();
		
		return vec2(i.x - v.x, i.y - v.y);
	}

	vec2 IGE_SceneView::GetMousePosInWindowNormalized()
	{
		vec2 i = GetMousePosInWindow();
		vec2 v = GetScreenSize();

		i.x = i.x != 0.0f ? i.x / v.x : 0.0f;
		i.y = i.y != 0.0f ? i.y / v.y : 0.0f;
		return i;
	}

	void IGE_SceneView::UpdateMouseControl()
	{

		//Copied from _interface.Inputs().Update()

		auto& app_sys = Core::GetSystem<Application>();
		//assert(currCamera);

		CameraControls& main_camera = Core::GetSystem<IDE>()._interface->Inputs()->main_camera;
		Handle<Camera> currCamera = main_camera.current_camera;
		Handle<Transform> tfm = currCamera->GetGameObject()->GetComponent<Transform>();
		//Please ignore all of this first
		constexpr auto cam_vel = 1.f;

		//WASD MOVEMENT
		if (app_sys.GetKey(Key::A))	tfm->position += -cam_vel * Core::GetRealDT().count() * tfm->Right();
		if (app_sys.GetKey(Key::D))	tfm->position += +cam_vel * Core::GetRealDT().count() * tfm->Right();
		//if (app_sys.GetKey(Key::S)) tfm->position += vec3{ 0, -0.016, 0.0 };
		//if (app_sys.GetKey(Key::W)) tfm->position += vec3{ 0, +0.016, 0.0 };
		if (app_sys.GetKey(Key::S))	tfm->position += +cam_vel * Core::GetRealDT().count() * tfm->Forward();
		if (app_sys.GetKey(Key::W))	tfm->position += -cam_vel * Core::GetRealDT().count() * tfm->Forward();
		if (app_sys.GetKey(Key::Q))	tfm->position += -cam_vel * Core::GetRealDT().count() * vec3 { 0, 1, 0 };
		if (app_sys.GetKey(Key::E))	tfm->position += +cam_vel * Core::GetRealDT().count() * vec3 { 0, 1, 0 };

		vec2 delta = ImGui::GetMouseDragDelta(1);

		//Order of multiplication Z*Y*X (ROLL*YAW*PITCH)

		//MOUSE YAW
		tfm->rotation = (quat{ vec3{0,1,0}, deg{90 * delta.x * -yaw_rotation_multiplier	} *Core::GetDT().count() } *tfm->rotation).normalize();
		//MOUSE PITCH
		tfm->rotation = (quat{ vec3{1,0,0}, deg{90 * delta.y * pitch_rotation_multiplier} *Core::GetDT().count() } *tfm->rotation).normalize();

		//This doesnt work as intended!!!!! ARGH
		//euler_angles cam_euler { tfm->rotation };
		//cam_euler.x += deg{ 90 * delta.y * pitch_rotation_multiplier };
		//cam_euler.y += deg{ 90 * delta.x * -yaw_rotation_multiplier };
		//tfm->rotation = quat{ cam_euler };

		//There is gimbal lock on the X axis when rotating on Y axis 90! 

		ImGui::ResetMouseDragDelta(1);


		//TEMP FIX ROLL
		if (app_sys.GetKey(Key::V)) tfm->rotation = (quat{ vec3{0,0,1}, deg{90} *Core::GetDT().count() } *tfm->rotation).normalize();
		if (app_sys.GetKey(Key::B)) tfm->rotation = (quat{ vec3{0,0,1}, deg{-90} *Core::GetDT().count() } *tfm->rotation).normalize();
		/**/


		//FOCUS BUTTON
		if (app_sys.GetKeyUp(Key::P))
		{
			static int _curr_cycle = 0;
			auto first = GameState::GetGameState().GetObjectsOfType<GameObject>()[_curr_cycle].GetHandle();
		
			if (first->HasComponent<Camera>())
			{
				_curr_cycle = (_curr_cycle + 1) % GameState::GetGameState().GetObjectsOfType<GameObject>().size();
				first = GameState::GetGameState().GetObjectsOfType<GameObject>()[_curr_cycle].GetHandle();
			}
			_curr_cycle = (_curr_cycle + 1) % GameState::GetGameState().GetObjectsOfType<GameObject>().size();
			//	currCamera->SetTarget(first->GetComponent<Transform>()->GlobalPosition());
			//	currCamera->Focus();
			main_camera.SetTarget(first->GetComponent<Transform>());
			//currCamera->LookAt(first->Transform()->GlobalPosition());
			//main_camera.LookAt();
		
			main_camera.Focus();
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

}
