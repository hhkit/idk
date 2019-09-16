//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_InspectorWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		16 SEPT 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <windows/IGE_InspectorWindow.h>
#include <editorstatic/imgui/imgui_internal.h> //InputTextEx
#include <app/Application.h>
#include <scene/SceneManager.h>
#include <IncludeComponents.h>
#include <IDE.h>
#include <iostream>
#include <ds/span.h>
#include <imgui/imgui_stl.h>
#include <math/euler_angles.h>

namespace idk {

	IGE_InspectorWindow::IGE_InspectorWindow()
		:IGE_IWindow{ "Inspector##IGE_InspectorWindow",true,ImVec2{ 300,600 },ImVec2{ 450,150 } } {		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoCollapse;




	}

	void IGE_InspectorWindow::BeginWindow()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{150.0f,100.0f});



	}
	void IGE_InspectorWindow::Update()
	{
		
		ImGui::PopStyleVar(1);


		IDE& editor = Core::GetSystem<IDE>();
		const size_t gameObjectsCount = editor.selected_gameObjects.size();
		if (gameObjectsCount == 1) {
			//Just show all components, Name and Transform first
			Handle<Name> c_name = editor.selected_gameObjects[0]->GetComponent<Name>();
			if (c_name) {
				DisplayNameComponent(c_name);

			}

			Handle<Transform> c_transform = editor.selected_gameObjects[0]->GetComponent<Transform>();
			if (c_transform) {
				DisplayTransformComponent(c_transform);
			}

			//Display remaining components here
			auto componentSpan = editor.selected_gameObjects[0]->GetComponents();
			if (componentSpan.size()) {
				for (auto& component : componentSpan) {
					if (component == c_name)
						continue;

					if (component == c_transform)
						continue;

					ImGui::PushID(static_cast<int>(component.id));
					auto componentName = (*component).type.name();
					if (ImGui::CollapsingHeader(string(componentName).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
					{



					}
					ImGui::PopID();
				}




			}



		}
		else if (gameObjectsCount > 1) {
			//Just show similar components

			//for (auto& i : editor.selected_gameObjects) {
			//
			//
			//	//Hierarchy Display
			//	SceneManager& sceneManager = Core::GetSystem<SceneManager>();
			//	SceneManager::SceneGraph& sceneGraph = sceneManager.FetchSceneGraph();
			//	sceneGraph.visit([](const Handle<GameObject>& handle, int depth) {
			//
			//
			//
			//		});
			//
			//}

		}







	}

	void IGE_InspectorWindow::DisplayNameComponent(Handle<Name>& c_name)
	{
		static string stringBuf{};

		Handle<GameObject> gameObject = c_name->GetGameObject();
		ImGui::Text("Name: ");
		ImGui::SameLine();
		if (ImGui::InputText("##Name", &stringBuf, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo)) {
			c_name->name = stringBuf;
		}
		//if (ImGui::IsItemDeactivatedAfterEdit()) {
		//}

		if (ImGui::IsItemClicked()) {
			stringBuf = c_name->name;
		}
		else if (!ImGui::IsItemActive()) { //Disable assignment when editing text
			stringBuf = c_name->name;
		}



		
		string idName = std::to_string(gameObject.id);
		ImGui::Text("ID: %s", idName.data());
		//ImGui::SameLine();
		//ImGui::InputText("##ID", &idName, ImGuiInputTextFlags_ReadOnly);
		//if (ImGui::InputText("##NAME", &selectedGameObject.lock()->name, ImGuiInputTextFlags_EnterReturnsTrue)) {
	}

	void IGE_InspectorWindow::DisplayTransformComponent(Handle<Transform>& c_transform)
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const float heightOffset = 2;
			const float widthOffset = 80;
			const float float3Size = 0.33f;
			const float float4Size = 0.25f;
			const float itemSpacing = 50;
			const float XYZSliderWidth = 10;

			//Position
			float heightPos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(heightPos + heightOffset);
			ImGui::Text("Position");
			ImGui::SameLine();
			ImGui::PushItemWidth(window_size.x * float3Size - itemSpacing);
			ImGui::SetCursorPosX(widthOffset);
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetCursorPosY(heightPos);
			ImGui::DragFloat("##PositionX", &c_transform->position.x);
			ImGui::SameLine();



			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::DragFloat("##PositionY", &c_transform->position.y);
			ImGui::SameLine();

			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::DragFloat("##PositionZ", &c_transform->position.z);

			//Rotation
			euler_angles original{ c_transform->rotation };


			heightPos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(heightPos + heightOffset);
			ImGui::Text("Rotation");
			ImGui::SameLine();
			ImGui::PushItemWidth(window_size.x * float3Size - itemSpacing);
			ImGui::SetCursorPosX(widthOffset);

			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetCursorPosY(heightPos);
			if (ImGui::SliderAngle("##RotationX", original.x.data())) {
				c_transform->rotation = quat{ original };
			}

			ImGui::SameLine();

			ImGui::Text("Y");
			ImGui::SameLine();
			if (ImGui::SliderAngle("##RotationY", original.y.data())) {
				c_transform->rotation = quat{ original };
			}

			ImGui::SameLine();

			ImGui::Text("Z");
			ImGui::SameLine();
			if (ImGui::SliderAngle("##RotationZ", original.z.data())) {
				c_transform->rotation = quat{ original };
			}


			//Scale
			heightPos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(heightPos + heightOffset);
			ImGui::Text("Scale");
			ImGui::SameLine();
			ImGui::PushItemWidth(window_size.x * float3Size - itemSpacing);
			ImGui::SetCursorPosX(widthOffset);

			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetCursorPosY(heightPos);
			ImGui::DragFloat("##ScaleX", &c_transform->scale.x);
			ImGui::SameLine();

			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::DragFloat("##ScaleY", &c_transform->scale.y);
			ImGui::SameLine();

			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::DragFloat("##ScaleZ", &c_transform->scale.z);



			ImGui::PopItemWidth();

		}
	}


}
