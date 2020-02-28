//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_MainWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		03 DEC 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <sstream> //sstream

#include <iostream>
#include <imgui/ImGuizmo.h>

#include <IDE.h>
#include <editor/windows/IGE_SceneView.h>
#include <editor/windows/IGE_HierarchyWindow.h>
#include <editor/commands/CommandList.h>
#include <editor/DragDropTypes.h>
#include <app/Application.h>
#include <scene/SceneManager.h>
#include <core/GameObject.inl>
#include <common/Transform.h> //transform
#include <gfx/MeshRenderer.h>
#include <gfx/RenderTarget.h>
#include <gfx/GraphicsSystem.h>
#include <prefab/PrefabUtility.h>
#include <vkn/VknFramebuffer.h>
#include <gfx/DebugRenderer.h>
#include <win32/WindowsApplication.h>
#include <math/matrix_transforms.inl>
#include <math/matrix_decomposition.inl>
#include <res/ResourceUtils.inl>

#include <opengl/system/OpenGLGraphicsSystem.h>
#include <opengl/PixelData.h>
#include <ds/result.inl>
#include <res/ResourceHandle.inl>
#include <anim/SkinnedMeshRenderer.h>
#include <gfx/MeshRenderer.h>

namespace idk {

	IGE_SceneView::IGE_SceneView()
		:IGE_IWindow{ "SceneView",true,ImVec2{ 800,600 },ImVec2{ 0,50 } } {		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoCollapse
					 | ImGuiWindowFlags_NoScrollWithMouse
					 | ImGuiWindowFlags_NoScrollbar
                     | ImGuiWindowFlags_MenuBar;

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
	namespace detail
	{
		template<typename T, typename Tuple, typename = void >
		struct is_in_tuple : std::false_type {};
		template<typename T, typename Tuple>
		struct is_in_tuple< T, Tuple,std::enable_if_t < index_in_tuple_v<T, Tuple> < std::tuple_size_v<Tuple>>> : std::true_type{};

		template<typename T>
		struct is_component_handle : std::false_type
		{

		};
		template<typename T,typename = void>
		struct is_component : std::false_type {};
		template<typename T>
		struct is_component<T, std::enable_if_t < index_in_tuple_v<T, Components> < std::tuple_size_v<Components>>> :std::true_type {};
		template<typename T>
		struct is_component_handle<Handle<T>> : is_component<T> {};

		template<typename T>
		struct HandleType;
		template<typename T>
		struct HandleType<Handle<T>> { using type = T; };
	}
	template<typename T>
	using HandleType_t = typename detail::HandleType<T>::type;
	template<typename T>
	constexpr inline bool is_component_handle_v = detail::is_component_handle<T>::value;
	template<typename T,typename Tuple>
	constexpr inline bool is_in_tuple_v = detail::is_in_tuple<T,Tuple>::value;

	using SelectableComponents = std::tuple< Handle<MeshRenderer>, Handle<SkinnedMeshRenderer>>;

	template<typename T>
	Handle<GameObject> GetGameObject([[maybe_unused]] T handle)
	{
		if constexpr (std::is_same_v<T, Handle<GameObject>>)
			return handle;
		else if constexpr (is_in_tuple_v<T, SelectableComponents>)
			return handle->GetGameObject();
		else
			return Handle<GameObject>{};
	}
	void IGE_SceneView::Update()
	{
        ImGui::PopStyleVar(3);

		auto& ide = Core::GetSystem<IDE>();

        if (!is_window_displayed)
        {
            ide.GetEditorCamera()->enabled = false;
            Core::GetSystem<GraphicsSystem>().enable_picking = false;
            return;
        }
        else
            ide.GetEditorCamera()->enabled = true;

		if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
			ide.DeleteSelectedGameObjects();

        if (ImGui::BeginMenuBar())
        {
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() -
                                 ImGui::CalcTextSize("Gizmos").x - ImGui::GetStyle().FramePadding.y * 2 -
                                 ImGui::GetTextLineHeight() - ImGui::GetStyle().ItemSpacing.x * 2);

            ImGui::Checkbox("Gizmos", &ide.GetEditorRenderTarget()->render_debug);
            ImGui::EndMenuBar();
        }

		ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMin().y);
		ImGui::BeginChild("##scenetex", {}, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
			ide.DeleteSelectedGameObjects();

		auto imageSize = vec2{ GetScreenSize() };
		auto meta = ide.GetEditorRenderTarget();
        auto screen_tex = meta->GetColorBuffer();
        auto rendertex_aspect = screen_tex->AspectRatio();
        auto windowframe_aspect = imageSize.x / imageSize.y;
        if (rendertex_aspect > windowframe_aspect) // rendertex is horizontally longer
            imageSize.x = imageSize.y * rendertex_aspect;
        else if (rendertex_aspect < windowframe_aspect) // windowframe is horizontally longer
            imageSize.y = imageSize.x / rendertex_aspect;
        draw_rect_size = imageSize;

        draw_rect_offset = (GetScreenSize() - imageSize) * 0.5f;

		//imageSize.y = (imageSize.x * (9 / 16));
		//if (Core::GetSystem<GraphicsSystem>().GetAPI() != GraphicsAPI::Vulkan)
		ImGui::SetCursorPos(draw_rect_offset);
		ImGui::Image(screen_tex->ID(), imageSize, ImVec2(0,1),ImVec2(1,0));

        if (ImGui::BeginDragDropTarget())
        {
			Handle<Camera> cam = ide.GetEditorCamera();
			vec3 objectFinalPos{};

			ray currRay = GenerateRayFromCurrentScreen();
			auto objs = Core::GetSystem<PhysicsSystem>().Raycast(currRay, LayerMask{ 0xFFFFFFFF });

			if (objs.size())
				objectFinalPos = objs[0].raycast_succ.point_of_collision; //WIP, does not work with spheres proper
			else
				objectFinalPos = currRay.origin + currRay.direction() * 10;

			// draw axes on where prefab will be spawned
			Core::GetSystem<DebugRenderer>().Draw(objectFinalPos, objectFinalPos + vec3(0, 0, 0.5f), color{ 0.0f,0.0f,1.0f });
			Core::GetSystem<DebugRenderer>().Draw(objectFinalPos, objectFinalPos + vec3(0.5f, 0, 0), color{ 1.0f,0.0f,0.0f });
			Core::GetSystem<DebugRenderer>().Draw(objectFinalPos, objectFinalPos + vec3(0, 0.5f, 0), color{ 0.0f,1.0f,0.0f });

            if (const auto* payload = ImGui::AcceptDragDropPayload(DragDrop::RESOURCE, ImGuiDragDropFlags_AcceptPeekOnly))
            {
                auto res_payload = DragDrop::GetResourcePayloadData();

                for (auto& h : res_payload)
                {
                    if (h.resource_id() != BaseResourceID<Prefab>)
                        continue;
                    if (!payload->IsDelivery())
                        continue;

                    auto* cmd = ide.ExecuteCommand<CMD_InstantiatePrefab>(h.AsHandle<Prefab>(), objectFinalPos);
					ide.SelectGameObject(cmd->GetGameObject(), false, true);
					ide.ExecuteCommand<CMD_CollateCommands>(2);
                    break;
                }
            }
            ImGui::EndDragDropTarget();
        }

		ImVec2 v = ImGui::GetWindowPos();

		ImVec2 currPos = ImGui::GetMousePosOnOpeningCurrentPopup();

		//Core::GetSystem<GraphicsSystem>().enable_picking = ImGui::IsMouseDown(0);


		//Raycast Selection
		std::optional< std::pair<GenericHandle,PickState>> result;

		if (ImGui::IsMouseReleased(0) && ImGui::IsWindowHovered() && !ImGui::GetIO().KeyAlt &&
			!ImGuizmo::IsUsing())
		{
			last_pick = 
			{
				Core::GetSystem<GraphicsSystem>().ColorPick(GetMousePosInWindowNormalized(), ide.GetEditorCamera()->GenerateCameraData()),
				PickState { .is_multi_select = ImGui::IsKeyDown(static_cast<int>(Key::Control)) }
			};
			
			ray currRay = GenerateRayFromCurrentScreen();
			vector<Handle<GameObject>> obj;
			if (Core::GetSystem<PhysicsSystem>().RayCastAllObj(currRay, obj))
			{
				//Sort the obj by closest dist to point
				//get the first obj
				auto& editor = ide;
				Handle<GameObject> closestGameObject = obj.front();
				auto cameraPos = editor.GetEditorCamera()->GetGameObject()->GetComponent<Transform>()->GlobalPosition();
				float distanceToCamera = closestGameObject->GetComponent<Transform>()->GlobalPosition().distance(cameraPos); //Setup first
				for (auto& iObject : obj)
				{
					float comparingDistance = cameraPos.distance(iObject->GetComponent<Transform>()->GlobalPosition());
					if (comparingDistance < distanceToCamera) 
					{
						closestGameObject = iObject;
						distanceToCamera = closestGameObject->GetComponent<Transform>()->GlobalPosition().distance(cameraPos); //Replace
					}
				}
				//result = closestGameObject;
			}
		}

		if (last_pick && last_pick->first.ready())
		{
			try
			{
				result = { last_pick->first.get(),last_pick->second };
			}
			catch (std::future_error& err)
			{
				LOG_TO(LogPool::EDIT, "Error \"%s\"when trying to get picked future ", err.what());
			}
			last_pick.reset();
		}
		
		if (result)
		{
			auto [closestRenderer,picked_state] = *result;
			Handle<GameObject> closestGameObject{};

			if (closestRenderer)
				closestGameObject = closestRenderer.visit([](auto handle) { return GetGameObject(handle); });

			if (closestGameObject)
			{
				auto& editor = ide;
				auto sel = editor.GetSelectedObjects();
				auto& selected_gameObjects = sel.game_objects;

				if (picked_state.is_multi_select) // select/deselect that particular handle
				{
					bool hasSelected = false;
					for (auto i = 0; i < selected_gameObjects.size(); ++i) //Select and deselect
					{
						if (closestGameObject == selected_gameObjects[i])
						{
							selected_gameObjects.erase(selected_gameObjects.begin() + i);
							ide.SetSelection(sel);
							hasSelected = true;
							break;
						}
					}
					if (!hasSelected)
					{
						selected_gameObjects.insert(selected_gameObjects.begin(), closestGameObject);
						ide.SetSelection(sel);
						editor.FindWindow<IGE_HierarchyWindow>()->ScrollToSelectedInHierarchy(closestGameObject);
					}
				}
				else //Select as normal
				{
					ide.SelectGameObject(closestGameObject);
					editor.FindWindow<IGE_HierarchyWindow>()->ScrollToSelectedInHierarchy(closestGameObject);
				}
			}
			else // If raycast hits nothing, we should clear the selected objects
			{
				ide.Unselect();
			}
		}

        POINT mouse_pos;
		GetCursorPos(&mouse_pos);
        currMouseScreenPos.x = mouse_pos.x;
        currMouseScreenPos.y = mouse_pos.y;

		//Right Mouse WASD control
		if (ImGui::IsMouseDown(1) && !ImGui::GetIO().KeyAlt) {
			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1)) { //Check if it is clicked here first!
				Handle<Transform> tfm = ide.GetEditorCamera()->GetGameObject()->GetComponent<Transform>();
				distance_to_orbit_point = orbit_point.distance(tfm->position);
				cachedMouseScreenPos = currMouseScreenPos;
				ImGui::SetWindowFocus();
				is_controlling_WASDcam = true;
			}	
		}
		else {
			is_controlling_WASDcam = false;
		}


		//Middle Mouse Pan control
		if (ImGui::IsMouseDown(2)) { //Doesnt matter if you hold alt or not!
			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(2)) { //Check if it is clicked here first!
				// MoveMouseToWindow();
				// GetCursorPos(&prevMouseScreenPos);
				cachedMouseScreenPos = currMouseScreenPos;
				ImGui::SetMouseCursor(ImGuiMouseCursor_None);
				ImGui::SetWindowFocus();
				is_controlling_Pancam = true;
			}
		}
		else {
			is_controlling_Pancam = false;
		}

		//ALT-RightClick Scroll
		if (ImGui::IsKeyDown(static_cast<int>(Key::Alt)) && ImGui::IsMouseDown(1)) {
			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1)) { //Check if it is clicked here first!
				cachedMouseScreenPos = currMouseScreenPos;
				ImGui::SetMouseCursor(ImGuiMouseCursor_None);
				ImGui::SetWindowFocus();
				is_controlling_ALTscroll = true;
			}
		}
		else {
			is_controlling_ALTscroll = false;
		}

		//ALT-LeftClick Orbit
		if (ImGui::IsKeyDown(static_cast<int>(Key::Alt)) && ImGui::IsMouseDown(0)) {
			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) { //Check if it is clicked here first!
				cachedMouseScreenPos = currMouseScreenPos;
				Handle<Transform> tfm = ide.GetEditorCamera()->GetGameObject()->GetComponent<Transform>();
				distance_to_orbit_point = orbit_point.distance(tfm->position);
				ImGui::SetMouseCursor(ImGuiMouseCursor_None);
				ImGui::SetWindowFocus();
				is_controlling_ALTorbit = true;
			}
		}
		else {
			is_controlling_ALTorbit = false;
		}



		if (is_controlling_ALTscroll) { //Alt controls take priority
			UpdateScrollMouseControl2();
		}
		else if (is_controlling_ALTorbit) {
			UpdateOrbitControl();
		}
		else if (is_controlling_WASDcam) {
			UpdateWASDMouseControl();
		}
		else if (is_controlling_Pancam) {
			UpdatePanMouseControl();
		}
		else {
			UpdateScrollMouseControl(); //Enable mouse control only if no WASD or Pan is in effect
		}
		prevMouseScreenPos = currMouseScreenPos;

		UpdateGizmoControl();
		DrawGlobalAxes();
		DrawSnapControl();
		//DrawGridControl();

		ImGui::EndChild();
	}

	vec2 IGE_SceneView::GetScreenSize()
	{
        //return vec2{ ImGui::GetWindowContentRegionMax() } - ImGui::GetWindowContentRegionMin();
        return vec2{ ImGui::GetWindowWidth(),ImGui::GetWindowHeight() };
	}

	vec2 IGE_SceneView::GetMousePosInWindow()
	{
		ImVec2 v = ImGui::GetWindowPos();
		ImVec2 i = ImGui::GetMousePos();
		
		return vec2(i.x - v.x, i.y - v.y);
	}

	vec2 IGE_SceneView::GetMousePosInWindowNormalized()
	{
		vec2 i = GetMousePosInWindow() - draw_rect_offset;
        const vec2 v = draw_rect_size;

		i.x = v.x != 0.0f ? i.x / v.x : 0.0f;
		i.y = v.y != 0.0f ? i.y / v.y : 0.0f;
		return i;
	}

	void IGE_SceneView::SetOrbitPoint(vec3 pt)
	{
		orbit_point = pt;
	}

	//void IGE_SceneView::DrawGridControl()
	//{
	//	ray newRay{};
	//	newRay.velocity.x = 100;
	//	Core::GetSystem<DebugRenderer>().Draw(newRay, color{ 1.0f,1.0f,1.0f,1 }, Core::GetDT());
	//}

	void IGE_SceneView::DrawSnapControl()
	{
		auto originalCursorPos = ImGui::GetCursorPos();
		auto windowWidth = ImGui::GetWindowContentRegionWidth();
		ImGui::SetCursorPos(ImVec2{ windowWidth - 158.0f - ImGui::CalcTextSize("Snapping ").x, 8.0f });
		std::stringstream translateTxt	{};
		std::stringstream rotateTxt		{};
		std::stringstream scaleTxt		{};
		translateTxt	<< "T[" << translate_snap_val[0]<<"]";
		rotateTxt		<< "R[" << rotate_snap_val		<<"]";
		scaleTxt		<< "S[" << scale_snap_val		<<"]";

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{});
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
		ImGui::Text("Snapping ");
		ImGui::SameLine();
		if (ImGui::Button(translateTxt.str().c_str(),ImVec2(50,0))) {
			ImGui::OpenPopup("TranslateSnap");
		}
		ImGui::SameLine();
		if (ImGui::Button(rotateTxt.str().c_str(), ImVec2(50, 0))) {
			ImGui::OpenPopup("RotateSnap");
		}
		ImGui::SameLine();
		if (ImGui::Button(scaleTxt.str().c_str(), ImVec2(50, 0))) {
			ImGui::OpenPopup("ScaleSnap");
		}

		ImGui::PopStyleVar(2);

		if (ImGui::BeginPopup("TranslateSnap")) {
			for (const auto& i : translate_snap_type) {
				std::stringstream txt{};
				txt << i;
				if (ImGui::RadioButton(txt.str().c_str(), translate_snap_val[0] == i)) {
					translate_snap_val[0] = translate_snap_val[1] = translate_snap_val[2] = i;
					ImGui::CloseCurrentPopup();
					break;
				}
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("RotateSnap")) {
			for (const auto& i : rotate_snap_type) {
				std::stringstream txt{};
				txt << i;
				if (ImGui::RadioButton(txt.str().c_str(), rotate_snap_val == i)) {
					rotate_snap_val = i;
					ImGui::CloseCurrentPopup();
					break;
				}
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("ScaleSnap")) {

			for (const auto& i : scale_snap_type) {
				std::stringstream txt{};
				txt << i;
				if (ImGui::RadioButton(txt.str().c_str(), scale_snap_val == i)) {
					scale_snap_val = i;
					ImGui::CloseCurrentPopup();
					break;
				}
			}

			ImGui::EndPopup();
		}

		ImGui::SetCursorPos(originalCursorPos);
	}

	void IGE_SceneView::DrawGlobalAxes()
	{
		const auto bottom_right = vec2(ImGui::GetWindowPos()) + vec2(ImGui::GetWindowContentRegionMax()) - vec2(32.0f, 32.0f);
		const float axis_len = 24.0f;
		IDE& editor = Core::GetSystem<IDE>();
		auto view = editor.GetEditorCamera()->ViewMatrix();

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

	void IGE_SceneView::UpdateWASDMouseControl()
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
		auto& app_sys = Core::GetSystem<Application>();

		const auto scroll = app_sys.GetMouseScroll().y;
		if (scroll > 0)
			cam_vel += cam_vel_additive * scroll;
		else if (scroll < 0)
			cam_vel = std::max(cam_vel + cam_vel_additive * scroll, min_cam_vel);

		Handle<Camera> currCamera = Core::GetSystem<IDE>().GetEditorCamera();
		Handle<Transform> tfm = currCamera->GetGameObject()->GetComponent<Transform>();

		const float finalCamVel = ImGui::GetIO().KeyShift ? cam_vel * cam_vel_shift_multiplier : cam_vel;
		//WASD MOVEMENT
		if (app_sys.GetKey(Key::A))	tfm->position += -finalCamVel * Core::GetRealDT().count() * tfm->Right();
		if (app_sys.GetKey(Key::D))	tfm->position += +finalCamVel * Core::GetRealDT().count() * tfm->Right();
		if (app_sys.GetKey(Key::S))	tfm->position += -finalCamVel * Core::GetRealDT().count() * tfm->Forward();
		if (app_sys.GetKey(Key::W))	tfm->position += +finalCamVel * Core::GetRealDT().count() * tfm->Forward();
		//VERTICAL MOVEMENT							  
		if (app_sys.GetKey(Key::Q))	tfm->position += -finalCamVel * Core::GetRealDT().count() * vec3 { 0, 1, 0 };
		if (app_sys.GetKey(Key::E))	tfm->position += +finalCamVel * Core::GetRealDT().count() * vec3 { 0, 1, 0 };

		//Mouse Controls
		vec2 delta;// = ImGui::GetMouseDragDelta(1, 0.1f);
		delta.x = static_cast<float>(currMouseScreenPos.x - prevMouseScreenPos.x);
		delta.y = static_cast<float>(currMouseScreenPos.y - prevMouseScreenPos.y);
		//ImGui::ResetMouseDragDelta(1);
		
		//Order of multiplication Z*Y*X (ROLL*YAW*PITCH)

		//MOUSE YAW
		tfm->rotation = (quat{ vec3{0,1,0}, deg{90 * delta.x * -yaw_rotation_multiplier	} *Core::GetDT().count() } *tfm->rotation).normalize(); //Global Rotation
		//MOUSE PITCH
		tfm->rotation = (tfm->rotation * quat{ vec3{1,0,0}, deg{90 * delta.y * -pitch_rotation_multiplier} *Core::GetDT().count() }).normalize(); //Local Rotation

		orbit_point = tfm->position + tfm->Forward() * distance_to_orbit_point;

		// MoveMouseToWindow();
		SetCursorPos(cachedMouseScreenPos.x, cachedMouseScreenPos.y);
		currMouseScreenPos = cachedMouseScreenPos;
	}

	void IGE_SceneView::UpdatePanMouseControl()
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);

		vec2 delta;
		delta.x = static_cast<float>(currMouseScreenPos.x - prevMouseScreenPos.x);
		delta.y = static_cast<float>(currMouseScreenPos.y - prevMouseScreenPos.y);

		Handle<Camera> currCamera = Core::GetSystem<IDE>().GetEditorCamera();
		Handle<Transform> tfm = currCamera->GetGameObject()->GetComponent<Transform>();
		vec3 localY =  tfm->Up()	* delta.y * pan_multiplier; //Amount to move in localy axis
		vec3 localX = -tfm->Right()	* delta.x * pan_multiplier; //Amount to move in localx axis
		tfm->position += localY;
		tfm->position += localX;
		orbit_point += localY;
		orbit_point += localX;

		SetCursorPos(cachedMouseScreenPos.x, cachedMouseScreenPos.y);
		currMouseScreenPos = cachedMouseScreenPos;
	}

	void IGE_SceneView::UpdateScrollMouseControl()
	{
		const auto scroll = Core::GetSystem<Application>().GetMouseScroll().y;
		auto tfm = Core::GetSystem<IDE>().GetEditorCamera()->GetGameObject()->Transform();

		if (ImGui::IsWindowHovered() && scroll)
			tfm->GlobalPosition(tfm->GlobalPosition() + tfm->Forward() * static_cast<float>(scroll));
	}

	void IGE_SceneView::UpdateScrollMouseControl2()
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);

		auto tfm = Core::GetSystem<IDE>().GetEditorCamera()->GetGameObject()->Transform();
		tfm->GlobalPosition(tfm->GlobalPosition() + tfm->Forward() * static_cast<float>(currMouseScreenPos.x - prevMouseScreenPos.x));

		SetCursorPos(cachedMouseScreenPos.x, cachedMouseScreenPos.y);
		currMouseScreenPos = cachedMouseScreenPos;
	}

	void IGE_SceneView::UpdateOrbitControl()
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);

		vec2 delta;
		delta.x = static_cast<float>(currMouseScreenPos.x - prevMouseScreenPos.x) * orbit_strength; //Global Y Axis
		delta.y = static_cast<float>(currMouseScreenPos.y - prevMouseScreenPos.y) * -orbit_strength; //Local X Axis

		auto tfm = Core::GetSystem<IDE>().GetEditorCamera()->GetGameObject()->GetComponent<Transform>();
		const vec3 orbit_pt_to_cam = tfm->position - orbit_point;
		const mat4 rotation_mat4 = rotate(vec3{ 0,1,0 }, rad(deg{ delta.x })) * rotate(tfm->Right(), rad(deg{ delta.y }));

		// normalize then mul by distance to preserve distance (in case of float calculation errors added over frames)
		tfm->position = orbit_point + (rotation_mat4 * vec4{ orbit_pt_to_cam, 1.0f }).normalize() * distance_to_orbit_point;
		Core::GetSystem<IDE>().GetEditorCamera()->LookAt(orbit_point);

		SetCursorPos(cachedMouseScreenPos.x, cachedMouseScreenPos.y);
		currMouseScreenPos = cachedMouseScreenPos;
	}

	void IGE_SceneView::UpdateGizmoControl()
	{
		//Getting camera datas
		IDE&				editor			 = Core::GetSystem<IDE>();
		Handle<Camera>		currCamera		 = editor.GetEditorCamera();
		Handle<Transform>	tfm				 = currCamera->GetGameObject()->GetComponent<Transform>();
		const auto			view_mtx		 = currCamera->ViewMatrix();
		const float*		viewMatrix		 = view_mtx.data();
		const auto			pers_mtx		 = currCamera->ProjectionMatrix();
		const float*		projectionMatrix = pers_mtx.data();

		//Setting up draw area

		ImVec2 winPos = vec2{ ImGui::GetWindowPos() } +vec2{ ImGui::GetWindowContentRegionMin() } +draw_rect_offset;
		ImGuizmo::SetRect(winPos.x, winPos.y, draw_rect_size.x, draw_rect_size.y); //The scene view size

		ImGuizmo::SetDrawlist(); //Draw on scene view only

		ImGuizmo::MODE gizmo_mode = editor.gizmo_mode == GizmoMode::Local ? ImGuizmo::MODE::LOCAL : ImGuizmo::MODE::WORLD;

		if (editor.GetSelectedObjects().game_objects.size() &&
			editor.GetSelectedObjects().game_objects[0] &&
			!ImGui::IsKeyDown(static_cast<int>(Key::Alt))) { //Dont enable gizmo when ALT is pressed prevent pressing on the gizmo when moving camera
			Handle<Transform> gameObjectTransform = editor.GetSelectedObjects().game_objects[0]->Transform();


			if (gameObjectTransform) {

				if (!ImGuizmo::IsUsing()) {
					auto matrix = gameObjectTransform->GlobalMatrix(); //Use the first object as Gizmo Control
					const float* temp = matrix.data();
					for (int i = 0; i < 16; ++i) {
						gizmo_matrix[i] = temp[i];
					}
				}

				switch (editor.gizmo_operation) 
				{
				case GizmoOperation::Translate:
					ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::TRANSLATE, gizmo_mode, gizmo_matrix, NULL, &translate_snap_val[0]);
					ImGuizmoManipulateUpdate(gameObjectTransform);
					break;
				case GizmoOperation::Rotate:
					ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::ROTATE,	gizmo_mode, gizmo_matrix, NULL, &rotate_snap_val);
					ImGuizmoManipulateUpdate(gameObjectTransform);
					break;
				case GizmoOperation::Scale:
					ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::SCALE,		gizmo_mode, gizmo_matrix, NULL, &scale_snap_val);
					ImGuizmoManipulateUpdate(gameObjectTransform);
					break;
				default:
				case GizmoOperation::Null:
					break;
				}

			}
		

			if (is_being_modified) {
				if (!ImGuizmo::IsUsing()) {
					int execute_counter = 0;
					for (int i = 0; i < editor.GetSelectedObjects().game_objects.size(); ++i) {
						mat4 modifiedMat = editor.GetSelectedObjects().game_objects[i]->GetComponent<Transform>()->GlobalMatrix();
						editor.ExecuteCommand<CMD_TransformGameObject>(editor.GetSelectedObjects().game_objects[i], original_matrices[i], modifiedMat);
						++execute_counter;
					}
					editor.ExecuteCommand<CMD_CollateCommands>(execute_counter);

					is_being_modified = false;
				}
			}


		}

	}

	void IGE_SceneView::CenterMouseInWindow()
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
		if (ImGuizmo::IsUsing())
		{
			IDE& editor = Core::GetSystem<IDE>();

			if (!is_being_modified)
			{
				original_matrices.clear();
				for (auto h : editor.GetSelectedObjects().game_objects)
					original_matrices.push_back(h->GetComponent<Transform>()->GlobalMatrix());
				is_being_modified = true;
			}

			mat4 difference = GenerateMat4FromGizmoMatrix() - gameObjectTransform->GlobalMatrix();
			for (int i = 0; i < editor.GetSelectedObjects().game_objects.size(); ++i)
			{
				Handle<Transform> iTransform = editor.GetSelectedObjects().game_objects[i]->GetComponent<Transform>();
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

	ray IGE_SceneView::WindowsPointToRay(vec2 vp_pos)
	{
		const auto view_mtx = Core::GetSystem<IDE>().GetEditorCamera()->ViewMatrix();
		const auto pers_mtx = Core::GetSystem<IDE>().GetEditorCamera()->ProjectionMatrix();

		//-1 to 1 (ndc)
		vp_pos -= vec2(0.5f, 0.5f);
		vp_pos /= 0.5f;

		vp_pos = vec2(vp_pos.x, -vp_pos.y);

		vec4 vfPos = pers_mtx.inverse() * vec4(vp_pos.x, vp_pos.y, 0, 1);
		vfPos *= 1.f / vfPos.w;
		vec4 wfPos = view_mtx.inverse() * vfPos;

		vec4 vbPos = pers_mtx.inverse() * vec4(vp_pos.x, vp_pos.y, 1, 1);
		vbPos *= 1.f / vbPos.w;
		vec4 wbPos = view_mtx.inverse() * vbPos;

		ray newRay;
		newRay.origin = wfPos;
		newRay.velocity = (wbPos - wfPos);

		return newRay;
	}

	ray IGE_SceneView::GenerateRayFromCurrentScreen()
	{
		return WindowsPointToRay(GetMousePosInWindowNormalized());
	}

	

}
