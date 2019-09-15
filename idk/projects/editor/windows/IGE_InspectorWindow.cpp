//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_InspectorWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		6 SEPT 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <editor/windows/IGE_InspectorWindow.h>
#include <editorstatic/imgui/imgui_internal.h> //InputTextEx
#include <app/Application.h>
#include <scene/SceneManager.h>
#include <IncludeComponents.h>
#include <IDE.h>
#include <iostream>
#include <imgui/imgui_stl.h>

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
		const int gameObjectsCount = editor.selected_gameObjects.size();
		if (gameObjectsCount == 1) {
			//Just show all components, Name and Transform first
			Handle<Name> c_name = editor.selected_gameObjects[0]->GetComponent<Name>();
			if (c_name) {
				ImGui::Text("Name: ");
				ImGui::SameLine();
				ImGui::InputText("##Name",&c_name->name,ImGuiInputTextFlags_EnterReturnsTrue);
				string idName = std::to_string(editor.selected_gameObjects[0].id);
				ImGui::Text("ID: %s", idName.data());
				//ImGui::SameLine();
				//ImGui::InputText("##ID", &idName, ImGuiInputTextFlags_ReadOnly);
				//if (ImGui::InputText("##NAME", &selectedGameObject.lock()->name, ImGuiInputTextFlags_EnterReturnsTrue)) {

			}

			Handle<Transform> c_transform = editor.selected_gameObjects[0]->GetComponent<Transform>();
			if (c_transform) {
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					const float heightOffset = 2;
					const float widthOffset  = 80;
					const float float3Size	 = 0.33f;
					const float float4Size	 = 0.25f;
					const float itemSpacing  = 50;

					//ImGui::Columns(2);

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
					ImGui::InputFloat("##PositionX", &c_transform->position.x);
					ImGui::SameLine();

					ImGui::Text("Y");
					ImGui::SameLine();
					ImGui::InputFloat("##PositionY", &c_transform->position.y);
					ImGui::SameLine();

					ImGui::Text("Z");
					ImGui::SameLine();
					ImGui::InputFloat("##PositionZ", &c_transform->position.z);

					//Rotation
					heightPos = ImGui::GetCursorPosY();
					ImGui::SetCursorPosY(heightPos + heightOffset);
					ImGui::Text("Rotation");
					ImGui::SameLine();
					ImGui::PushItemWidth(window_size.x * float4Size - itemSpacing);
					ImGui::SetCursorPosX(widthOffset);

					ImGui::Text("X");
					ImGui::SameLine();
					ImGui::SetCursorPosY(heightPos);
					ImGui::InputFloat("##RotationX", &c_transform->rotation.x);
					ImGui::SameLine();

					ImGui::Text("Y");
					ImGui::SameLine();
					ImGui::InputFloat("##RotationY", &c_transform->rotation.y);
					ImGui::SameLine();

					ImGui::Text("Z");
					ImGui::SameLine();
					ImGui::InputFloat("##RotationZ", &c_transform->rotation.z);
					ImGui::SameLine();

					ImGui::Text("W");
					ImGui::SameLine();
					ImGui::InputFloat("##RotationW", &c_transform->rotation.w);

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
					ImGui::InputFloat("##ScaleX", &c_transform->scale.x);
					ImGui::SameLine();

					ImGui::Text("Y");
					ImGui::SameLine();
					ImGui::InputFloat("##ScaleY", &c_transform->scale.y);
					ImGui::SameLine();

					ImGui::Text("Z");
					ImGui::SameLine();
					ImGui::InputFloat("##ScaleZ", &c_transform->scale.z);



					ImGui::PopItemWidth();

				}
			}

			//Display remaining components here

			//if 
			//editor.selected_gameObjects[0]->GetComponents();



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

}
