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
#include <imgui/ImGuizmo.h>
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

		//Right Mouse WASD control
		if (ImGui::IsMouseDown(1)) {
			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1)) { //Check if it is clicked here first!
				ImGui::SetWindowFocus();
				is_controlling_WASDcam = true;
			}
		}
		else {
			is_controlling_WASDcam = false;
		}

		

		//Middle Mouse Pan control
		if (ImGui::IsMouseDown(2)) {
			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(2)) { //Check if it is clicked here first!
				ImGui::SetWindowFocus();
				is_controlling_Pancam = true;
			}
		}
		else {
			is_controlling_Pancam = false;
		}

		if (is_controlling_WASDcam) {
			UpdateWASDMouseControl();
		}
		else if (is_controlling_Pancam) {
			UpdatePanMouseControl();

		}


		UpdateGizmoControl();


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

	void IGE_SceneView::UpdateWASDMouseControl()
	{

		//Copied from _interface.Inputs().Update()

		auto& app_sys = Core::GetSystem<Application>();

		CameraControls& main_camera = Core::GetSystem<IDE>()._interface->Inputs()->main_camera;
		Handle<Camera> currCamera = main_camera.current_camera;
		Handle<Transform> tfm = currCamera->GetGameObject()->GetComponent<Transform>();

		//Left shift = 16
		const float finalCamVel = ImGui::IsKeyDown(16) ? cam_vel * cam_vel_shift_multiplier : cam_vel;
		//WASD MOVEMENT
		if (app_sys.GetKey(Key::A))	tfm->position += -finalCamVel * Core::GetRealDT().count() * tfm->Right();
		if (app_sys.GetKey(Key::D))	tfm->position += +finalCamVel * Core::GetRealDT().count() * tfm->Right();
		if (app_sys.GetKey(Key::S))	tfm->position += +finalCamVel * Core::GetRealDT().count() * tfm->Forward();
		if (app_sys.GetKey(Key::W))	tfm->position += -finalCamVel * Core::GetRealDT().count() * tfm->Forward();
		//VERTICAL MOVEMENT							  
		if (app_sys.GetKey(Key::Q))	tfm->position += -finalCamVel * Core::GetRealDT().count() * vec3 { 0, 1, 0 };
		if (app_sys.GetKey(Key::E))	tfm->position += +finalCamVel * Core::GetRealDT().count() * vec3 { 0, 1, 0 };

		vec2 delta = ImGui::GetMouseDragDelta(1,0.1f);

		//Order of multiplication Z*Y*X (ROLL*YAW*PITCH)

		//MOUSE YAW
		tfm->rotation = (quat{ vec3{0,1,0}, deg{90 * delta.x * -yaw_rotation_multiplier	} *Core::GetDT().count() } *tfm->rotation).normalize(); //Global Rotation
		//MOUSE PITCH
		tfm->rotation = (tfm->rotation * quat{ vec3{1,0,0}, deg{90 * delta.y * -pitch_rotation_multiplier} *Core::GetDT().count() }).normalize(); //Local Rotation



		ImGui::ResetMouseDragDelta(1);


	}

	void IGE_SceneView::UpdatePanMouseControl()
	{
		vec2 delta = ImGui::GetMouseDragDelta(2,0.1f);

		auto& app_sys = Core::GetSystem<Application>();

		CameraControls& main_camera = Core::GetSystem<IDE>()._interface->Inputs()->main_camera;
		Handle<Camera> currCamera = main_camera.current_camera;
		Handle<Transform> tfm = currCamera->GetGameObject()->GetComponent<Transform>();
		vec3 localY = tfm->Up()* delta.y*-pan_multiplier; //Amount to move in localy axis
		vec3 localX = tfm->Right()* delta.x* pan_multiplier; //Amount to move in localx axis
		tfm->position += localY;
		tfm->position += localX;

		ImGui::ResetMouseDragDelta(2);

	}

	void IGE_SceneView::UpdateGizmoControl()
	{
		//Getting camera datas
		IDE& editor = Core::GetSystem<IDE>();
		CameraControls& main_camera = editor._interface->Inputs()->main_camera;
		Handle<Camera> currCamera = main_camera.current_camera;
		Handle<Transform> tfm = currCamera->GetGameObject()->GetComponent<Transform>();
		float* viewMatrix = currCamera->ViewMatrix().data();
		float* projectionMatrix = currCamera->ProjectionMatrix().data();

		//Setting up draw area
		ImGuiIO& io = ImGui::GetIO();

		ImVec2 winPos = ImGui::GetWindowPos();
		winPos.y = ImGui::GetFrameHeight();
		ImGuizmo::SetRect(winPos.x, winPos.y, GetScreenSize().x, GetScreenSize().y); //The scene view size
		ImGuizmo::SetDrawlist(); //Draw on scene view only

		ImGuizmo::MODE gizmo_mode = editor.gizmo_mode == MODE::LOCAL ? ImGuizmo::MODE::LOCAL : ImGuizmo::MODE::WORLD;

		if (editor.selected_gameObjects.size()) {
			if (editor.selected_gameObjects.size() == 1) {
				Handle<Transform> gameObjectTransform = editor.selected_gameObjects[0]->GetComponent<Transform>();
				if (gameObjectTransform) {

					switch (editor.gizmo_operation) {
					default:
					case GizmoOperation_Null:
						break;
					case GizmoOperation_Translate:
						ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::TRANSLATE, gizmo_mode, gameObjectTransform->GlobalMatrix().data(), NULL, NULL);

						break;
					case GizmoOperation_Rotate:
						ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::ROTATE, gizmo_mode, gameObjectTransform->GlobalMatrix().data(), NULL, NULL);

						break;
					case GizmoOperation_Scale:
						ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::SCALE, gizmo_mode, gameObjectTransform->GlobalMatrix().data(), NULL, NULL);

						break;
					}
				}
			}
			else {
				//For multiple objects
			}

		}
	}

}
