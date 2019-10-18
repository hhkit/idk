//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_MainWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		02 OCT 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <imgui/ImGuizmo.h>
#include <editor/windows/IGE_SceneView.h>
#include <editor/commands/CommandList.h>
#include <editor/DragDropTypes.h>
#include <IDE.h>
#include <app/Application.h>
#include <core/GameObject.h>
#include <common/Transform.h> //transform
#include <gfx/RenderTarget.h>
#include <gfx/GraphicsSystem.h>
#include <prefab/PrefabUtility.h>
#include <vkn/VknFramebuffer.h>
#include <gfx/DebugRenderer.h>
#include <win32/WindowsApplication.h>

#include <phys/PhysicsSystem.h>

#include <iostream>

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

        ImVec2 imageSize = GetScreenSize();
        auto screen_tex = RscHandle<RenderTarget>{}->GetMeta().textures[0];
        auto rendertex_aspect = screen_tex->AspectRatio();
        auto windowframe_aspect = imageSize.x / imageSize.y;
        if (rendertex_aspect > windowframe_aspect) // rendertex is horizontally longer
            imageSize.x = imageSize.y * rendertex_aspect;
        else if (rendertex_aspect < windowframe_aspect) // windowframe is horizontally longer
            imageSize.y = imageSize.x / rendertex_aspect;
        draw_rect_size = imageSize;

        draw_rect_offset = (GetScreenSize() - imageSize) * 0.5f;
        ImGui::SetCursorPos(vec2(ImGui::GetWindowContentRegionMin()) + draw_rect_offset);

		//imageSize.y = (imageSize.x * (9 / 16));
		//if (Core::GetSystem<GraphicsSystem>().GetAPI() != GraphicsAPI::Vulkan)
		ImGui::Image(screen_tex->ID(), imageSize, ImVec2(0,1),ImVec2(1,0));

        if (ImGui::BeginDragDropTarget())
        {
            if (const auto* payload = ImGui::AcceptDragDropPayload(DragDrop::RESOURCE, ImGuiDragDropFlags_AcceptPeekOnly))
            {
                auto res_payload = DragDrop::GetResourcePayloadData();

                for (auto& h : res_payload)
                {
                    if (h.resource_id() != BaseResourceID<Prefab>)
                        continue;
                    if (!payload->IsDelivery())
                        continue;

                    auto go = PrefabUtility::Instantiate(h.AsHandle<Prefab>(), *Core::GetSystem<SceneManager>().GetActiveScene());
                    Handle<Camera> cam = Core::GetSystem<IDE>()._interface->Inputs()->main_camera.current_camera;
                    go->Transform()->position = cam->currentPosition() + cam->GetGameObject()->Transform()->Forward();
                    break;
                }
            }
            ImGui::EndDragDropTarget();
        }

		ImVec2 v = ImGui::GetWindowPos();

		ImVec2 currPos = ImGui::GetMousePosOnOpeningCurrentPopup();


		//Raycast Selection
		if (ImGui::IsMouseReleased(0) && ImGui::IsWindowHovered() && !ImGuizmo::IsOver() && !ImGuizmo::IsUsing()) {

			//Use this function to get
			//std::cout << "MousePosInWindow: " << GetMousePosInWindowNormalized().x << "," << GetMousePosInWindowNormalized().y << "\n";

			//Select gameobject here!
			currRay = GenerateRayFromCurrentScreen();
			vector<Handle<GameObject>> obj;
			if (Core::GetSystem<PhysicsSystem>().RayCastAllObj(currRay, obj))
			{
				//Sort the obj by closest dist to point
				//get the first obj
				auto& editor = Core::GetSystem<IDE>();
				vector<Handle<GameObject>>& selected_gameObjects = editor.selected_gameObjects; //Get reference from IDE
				Handle<GameObject> closestGameObject = obj.front();
				auto cameraPos = editor.currentCamera().current_camera->currentPosition();
				float distanceToCamera = closestGameObject->GetComponent<Transform>()->position.distance(cameraPos); //Setup first
				for (auto& iObject : obj) {
					float comparingDistance = cameraPos.distance(iObject->GetComponent<Transform>()->position);
					if (comparingDistance < distanceToCamera) {
						closestGameObject = iObject;
						distanceToCamera = closestGameObject->GetComponent<Transform>()->position.distance(cameraPos); //Replace
					}
				}


				if (ImGui::IsKeyDown(static_cast<int>(Key::Control))) { //Or select Deselect that particular handle
					bool hasSelected = false;
					for (auto counter = 0; counter < selected_gameObjects.size(); ++counter) { //Select and deselect
						if (closestGameObject == selected_gameObjects[counter]) {
							selected_gameObjects.erase(selected_gameObjects.begin() + counter);
							hasSelected = true;
							break;
						}
					}
					if (!hasSelected)
						selected_gameObjects.insert(selected_gameObjects.begin(),closestGameObject);
					Core::GetSystem<IDE>().RefreshSelectedMatrix();

				}
				else {
					//Select as normal
					selected_gameObjects.clear();
					selected_gameObjects.push_back(closestGameObject);
					Core::GetSystem<IDE>().RefreshSelectedMatrix();
				}


			}
		}

		//Right Mouse WASD control
		if (ImGui::IsMouseDown(1)) {
			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1)) { //Check if it is clicked here first!
				MoveMouseToWindow();
				GetCursorPos(&prevMouseScreenPos);
				GetCursorPos(&currMouseScreenPos);
				ImGui::SetMouseCursor(ImGuiMouseCursor_None);
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
				MoveMouseToWindow();
				GetCursorPos(&prevMouseScreenPos);
				GetCursorPos(&currMouseScreenPos);
				ImGui::SetMouseCursor(ImGuiMouseCursor_None);
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

		Core::GetSystem<DebugRenderer>().Draw(currRay, color{ 0,0.8f,0,1 }, Core::GetDT());

		UpdateScrollMouseControl();

		UpdateGizmoControl();

		// draw current global axes
		const auto bottom_right = vec2(ImGui::GetWindowPos()) + vec2(ImGui::GetWindowContentRegionMax()) - vec2(32.0f, 32.0f);
		const float axis_len = 24.0f;
		IDE& editor = Core::GetSystem<IDE>();
		CameraControls& main_camera = editor._interface->Inputs()->main_camera;
		Handle<Camera> currCamera = main_camera.current_camera;
		Handle<Transform> tfm = currCamera->GetGameObject()->GetComponent<Transform>();
		auto view = currCamera->ViewMatrix();

		vec2 right = (view * vec4(1.0f, 0, 0, 0)).xy;
		right.x *= -1;
		vec2 up = (view * vec4(0, 1.0f, 0, 0)).xy;
		vec3 forward = view * vec4(0, 0, 1.0f, 0);
		forward.x *= -1;
		vec2 forw = forward.xy;
		if (forward.z < 0)
			ImGui::GetWindowDrawList()->AddLine(bottom_right, bottom_right - axis_len * forw, 0xffff0000, 2.0f);
		ImGui::GetWindowDrawList()->AddLine(bottom_right, bottom_right - axis_len * right, 0xff0000ff, 2.0f);
		ImGui::GetWindowDrawList()->AddLine(bottom_right, bottom_right - axis_len * up, 0xff00ff00, 2.0f);
		if (forward.z > 0)
			ImGui::GetWindowDrawList()->AddLine(bottom_right, bottom_right - axis_len * forw, 0xffff0000, 2.0f);


	}

	void IGE_SceneView::SetTexture(void* textureToRender)
	{
		sceneTexture = textureToRender;
	}

	vec2 IGE_SceneView::GetScreenSize()
	{
        return vec2{ ImGui::GetWindowContentRegionMax() } - ImGui::GetWindowContentRegionMin();
        // vec2{ ImGui::GetWindowWidth(),ImGui::GetWindowHeight() - ImGui::GetFrameHeight() };
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
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
		auto& app_sys = Core::GetSystem<Application>();

		CameraControls& main_camera = Core::GetSystem<IDE>()._interface->Inputs()->main_camera;
		Handle<Camera> currCamera = main_camera.current_camera;
		Handle<Transform> tfm = currCamera->GetGameObject()->GetComponent<Transform>();

		//Left shift = 16
		const float finalCamVel = ImGui::IsKeyDown(16) ? cam_vel * cam_vel_shift_multiplier : cam_vel;
		//WASD MOVEMENT
		if (app_sys.GetKey(Key::A))	tfm->position += -finalCamVel * Core::GetRealDT().count() * tfm->Right();
		if (app_sys.GetKey(Key::D))	tfm->position += +finalCamVel * Core::GetRealDT().count() * tfm->Right();
		if (app_sys.GetKey(Key::S))	tfm->position += -finalCamVel * Core::GetRealDT().count() * tfm->Forward();
		if (app_sys.GetKey(Key::W))	tfm->position += +finalCamVel * Core::GetRealDT().count() * tfm->Forward();
		//VERTICAL MOVEMENT							  
		if (app_sys.GetKey(Key::Q))	tfm->position += -finalCamVel * Core::GetRealDT().count() * vec3 { 0, 1, 0 };
		if (app_sys.GetKey(Key::E))	tfm->position += +finalCamVel * Core::GetRealDT().count() * vec3 { 0, 1, 0 };

		//Mouse Controls

		GetCursorPos(&currMouseScreenPos);
		//int drag_X{};
		//int drag_Y{};
		//drag_X = currPos.x - prevPos.x;
		//drag_Y = currPos.y - prevPos.y;
		//std::cout << "Drag:" << drag_X << "," << drag_Y << std::endl;

		vec2 delta{};// = ImGui::GetMouseDragDelta(1, 0.1f);
		delta.x = static_cast<float>(currMouseScreenPos.x - prevMouseScreenPos.x);
		delta.y = static_cast<float>(currMouseScreenPos.y - prevMouseScreenPos.y);
		//ImGui::ResetMouseDragDelta(1);

		//Order of multiplication Z*Y*X (ROLL*YAW*PITCH)

		//MOUSE YAW
		tfm->rotation = (quat{ vec3{0,1,0}, deg{90 * delta.x * -yaw_rotation_multiplier	} *Core::GetDT().count() } *tfm->rotation).normalize(); //Global Rotation
		//MOUSE PITCH
		tfm->rotation = (tfm->rotation * quat{ vec3{1,0,0}, deg{90 * delta.y * -pitch_rotation_multiplier} *Core::GetDT().count() }).normalize(); //Local Rotation

		MoveMouseToWindow();

		GetCursorPos(&prevMouseScreenPos);
	}

	void IGE_SceneView::UpdatePanMouseControl()
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);

		//vec2 delta = ImGui::GetMouseDragDelta(2,0.1f);
		GetCursorPos(&currMouseScreenPos);
		//int drag_X{};
		//int drag_Y{};
		//drag_X = currPos.x - prevPos.x;
		//drag_Y = currPos.y - prevPos.y;
		//std::cout << "Drag:" << drag_X << "," << drag_Y << std::endl;

		vec2 delta{};
		delta.x = static_cast<float>(currMouseScreenPos.x - prevMouseScreenPos.x);
		delta.y = static_cast<float>(currMouseScreenPos.y - prevMouseScreenPos.y);
		//auto& app_sys = Core::GetSystem<Application>();
		IDE& editor = Core::GetSystem<IDE>();

		CameraControls& main_camera = Core::GetSystem<IDE>()._interface->Inputs()->main_camera;
		Handle<Camera> currCamera = main_camera.current_camera;
		Handle<Transform> tfm = currCamera->GetGameObject()->GetComponent<Transform>();
		vec3 localY = tfm->Up()* delta.y*pan_multiplier		* editor.scroll_multiplier* 0.5f; //Amount to move in localy axis
		vec3 localX = -tfm->Right()* delta.x* pan_multiplier* editor.scroll_multiplier* 0.5f; //Amount to move in localx axis
		tfm->position += localY;
		tfm->position += localX;

		//ImGui::ResetMouseDragDelta(2);

		MoveMouseToWindow();

		GetCursorPos(&prevMouseScreenPos);

	}

	void IGE_SceneView::UpdateScrollMouseControl()
	{
		
		auto scroll = Core::GetSystem<Application>().GetMouseScroll().y;
		auto cam = Core::GetSystem<IDE>()._interface->Inputs()->main_camera;
		auto tfm = cam.current_camera->GetGameObject()->Transform();
		IDE& editor = Core::GetSystem<IDE>();
		if (ImGui::IsWindowHovered() && abs(scroll) > epsilon)
			tfm->GlobalPosition(tfm->GlobalPosition() + tfm->Forward() * (scroll / float{ 12000 }) * editor.scroll_multiplier);

		if (scroll > 0)
			editor.DecreaseScrollPower();
		else if (scroll < 0)
			editor.IncreaseScrollPower();

	}

	void IGE_SceneView::UpdateGizmoControl()
	{
		//Getting camera datas
		IDE& editor = Core::GetSystem<IDE>();
		CameraControls& main_camera = editor._interface->Inputs()->main_camera;
		Handle<Camera> currCamera = main_camera.current_camera;
		Handle<Transform> tfm = currCamera->GetGameObject()->GetComponent<Transform>();
		const auto view_mtx = currCamera->ViewMatrix();
		const float* viewMatrix = view_mtx.data();
		const auto pers_mtx = currCamera->ProjectionMatrix();
		const float* projectionMatrix = pers_mtx.data();

		//Setting up draw area
		//ImGuiIO& io = ImGui::GetIO();

        ImVec2 winPos = vec2{ ImGui::GetWindowPos() } + ImGui::GetWindowContentRegionMin() + draw_rect_offset;
		ImGuizmo::SetRect(winPos.x, winPos.y, draw_rect_size.x, draw_rect_size.y); //The scene view size

		ImGuizmo::SetDrawlist(); //Draw on scene view only

		ImGuizmo::MODE gizmo_mode = editor.gizmo_mode == MODE::LOCAL ? ImGuizmo::MODE::LOCAL : ImGuizmo::MODE::WORLD;

		if (editor.selected_gameObjects.size()) {
			Handle<Transform> gameObjectTransform = editor.selected_gameObjects[0]->GetComponent<Transform>(); 
			if (gameObjectTransform) {

				if (!ImGuizmo::IsUsing()) {
					auto matrix = gameObjectTransform->GlobalMatrix(); //Use the first object as Gizmo Control
					const float* temp = matrix.data();
					for (int i = 0; i < 16; ++i) {
						gizmo_matrix[i] = temp[i];
					}
				}

				switch (editor.gizmo_operation) {
				default:
				case GizmoOperation_Null:

					break;

				case GizmoOperation_Translate:
					ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::TRANSLATE, gizmo_mode, gizmo_matrix, NULL, NULL);
					ImGuizmoManipulateUpdate(gameObjectTransform);
					break;

				case GizmoOperation_Rotate:
					ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::ROTATE,	gizmo_mode, gizmo_matrix, NULL, NULL);
					ImGuizmoManipulateUpdate(gameObjectTransform);
					break;

				case GizmoOperation_Scale:
					ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::SCALE,		gizmo_mode, gizmo_matrix, NULL, NULL);
					ImGuizmoManipulateUpdate(gameObjectTransform);
					break;
				}

			}
		

			if (is_being_modified) {
				if (!ImGuizmo::IsUsing()) {
					vector<mat4>& originalMatrix = editor.selected_matrix;
					for (int i = 0; i < editor.selected_gameObjects.size(); ++i) {
						mat4 modifiedMat = editor.selected_gameObjects[i]->GetComponent<Transform>()->GlobalMatrix();
						editor.command_controller.ExecuteCommand(COMMAND(CMD_TransformGameObject, editor.selected_gameObjects[i], originalMatrix[i], modifiedMat));
					}
					//Refresh the new matrix values
					editor.RefreshSelectedMatrix();

					is_being_modified = false;
				}
			}


		}
	}

	void IGE_SceneView::MoveMouseToWindow()
	{

		RECT rect = { NULL };
		GetWindowRect(Core::GetSystem<Windows>().GetWindowHandle(), &rect);
		int x = static_cast<int>(rect.left);
		int y = static_cast<int>(rect.top)+20; //offset of window header
		
		x += static_cast<int>(round(ImGui::GetWindowPos().x)		);
		y += static_cast<int>(round(ImGui::GetWindowPos().y)		);
		x += static_cast<int>(round(ImGui::GetWindowSize().x * 0.5f));
		y += static_cast<int>(round(ImGui::GetWindowSize().y * 0.5f));
		SetCursorPos(x, y);

	}

	void IGE_SceneView::ImGuizmoManipulateUpdate(Handle<Transform>& gameObjectTransform)
	{
		if (ImGuizmo::IsUsing()) {
			is_being_modified = true;
			mat4 difference = GenerateMat4FromGizmoMatrix() - gameObjectTransform->GlobalMatrix();
			IDE& editor = Core::GetSystem<IDE>();
			for (int i = 0; i < editor.selected_gameObjects.size(); ++i) {
				Handle<Transform> iTransform = editor.selected_gameObjects[i]->GetComponent<Transform>();
				mat4 newMatrix = iTransform->GlobalMatrix() + difference;
				iTransform->GlobalMatrix(newMatrix); //Assign new variables
			}
		}
	}

	mat4 IGE_SceneView::GenerateMat4FromGizmoMatrix()
	{
		mat4 dumper{}; //Using like this --> mat4 dumper{gizmo_matrix} does not yield the same result!
		int i = 0;
		for (int row = 0; row < 4; ++row) {
			for (int col = 0; col < 4; ++col) {
				dumper[row][col] = gizmo_matrix[i];
				++i;
			}
		}
		return dumper;
	}

	ray IGE_SceneView::GenerateRayFromCurrentScreen()
	{
		//auto& app_sys = Core::GetSystem<Application>();
		IDE& editor = Core::GetSystem<IDE>();

		CameraControls& main_camera = Core::GetSystem<IDE>()._interface->Inputs()->main_camera;
		Handle<Camera> currCamera = main_camera.current_camera;

		const auto view_mtx = currCamera->ViewMatrix();
		const auto pers_mtx = currCamera->ProjectionMatrix();
		
		vec2 ndcPos = GetMousePosInWindowNormalized();

		//-1 to 1 (ndc)
		ndcPos -= vec2(0.5f,0.5f);
		ndcPos /= 0.5f;

		ndcPos = vec2(ndcPos.x,-ndcPos.y);

		vec4 vfPos = pers_mtx.inverse() * vec4(ndcPos.x,ndcPos.y,0,1);
		vfPos *= 1.f / vfPos.w;
		vec4 wfPos = view_mtx.inverse() * vfPos;

		vec4 vbPos = pers_mtx.inverse() * vec4(ndcPos.x, ndcPos.y, 1, 1);
		vbPos *= 1.f/vbPos.w;
		vec4 wbPos = view_mtx.inverse() * vbPos;

		ray newRay;
		newRay.origin = wfPos;
		newRay.velocity = (wbPos - wfPos);

		return newRay;
	}

}
