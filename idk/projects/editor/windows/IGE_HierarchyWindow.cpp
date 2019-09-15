//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_HierarchyWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		9 SEPT 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <editor/windows/IGE_HierarchyWindow.h>
#include <editor/commands/CommandList.h>		//Commands
#include <editorstatic/imgui/imgui_internal.h> //InputTextEx
#include <app/Application.h>
#include <scene/SceneManager.h>
#include <core/GameObject.h>
#include <common/Name.h>
#include <core/Core.h>
#include <IDE.h>		//IDE
#include <iostream>

namespace idk {

	IGE_HierarchyWindow::IGE_HierarchyWindow()
		:IGE_IWindow{ "Hierarchy##IGE_HierarchyWindow",true,ImVec2{ 300,600 },ImVec2{ 150,150 } } {		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoCollapse;




	}

	void IGE_HierarchyWindow::BeginWindow()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{150.0f,100.0f});

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));


	}
	void IGE_HierarchyWindow::Update()
	{
		
		ImGui::PopStyleVar(2);





		ImGuiStyle& style = ImGui::GetStyle();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_TitleBgActive]);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		//Tool bar

		ImGui::SetCursorPos(ImVec2{ 0.0f,ImGui::GetFrameHeight() });


		//Toolbar
		const ImVec2 toolBarSize{ window_size.x, 18.0f };
		const ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoCollapse;
		ImGui::BeginChild("HierarchyToolBar", toolBarSize, false, childFlags);
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(3);

		ImGui::SetCursorPosX(5);
		if (ImGui::Button("Create")) {
			ImGui::OpenPopup("CreatePopup");


		}

		if (ImGui::BeginPopup("CreatePopup")) {
			if (ImGui::MenuItem("Create Empty")) {
				IDE& editor = Core::GetSystem<IDE>();
				editor.command_controller.ExecuteCommand(COMMAND(CMD_CreateGameObject));


			}

			ImGui::EndPopup();
		}


		ImGui::SameLine();
		ImVec2 startPos = ImGui::GetCursorPos();
		ImGui::SetCursorPosX(startPos.x+15);

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
		static char searchBarChar[512];
		if (ImGui::InputTextEx("##ToolBarSearchBar",NULL, searchBarChar, 512, ImVec2{window_size.x-100,ImGui::GetFrameHeight()-2}, ImGuiInputTextFlags_None)) {
			//Do something
		}
		ImGui::PopStyleVar();

		ImGui::EndChild();

		//Hierarchy Display
		SceneManager& sceneManager = Core::GetSystem<SceneManager>();
		SceneManager::SceneGraph& sceneGraph = sceneManager.FetchSceneGraph();
		
		
		//Refer to TestSystemManager.cpp
		sceneGraph.visit([](const Handle<GameObject>& handle, int depth) {
			if (!handle)
				return;

			//This indent is temporary, will integrate with ImGui::Tree later TODO
			for (int i = 0; i < depth; ++i)
				ImGui::Indent();

			vector<Handle<GameObject>>& selected_gameObjects = Core::GetSystem<IDE>().selected_gameObjects;

			
			//handle->Transform()->SetParent(parent, true);
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

			//Check if gameobject has been selected. Causes Big-O(n^2)
			for (Handle<GameObject>& i : selected_gameObjects) {
				if (handle == i) {
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
					break;
				}
			}
			Handle<Name> c_name = handle->GetComponent<Name>();
			string goName{};
			if (c_name)
				goName = c_name->name;
			
			bool isNameEmpty = goName.empty();
			if (isNameEmpty) {
				goName = "Unnamed (";
				goName.append(std::to_string(handle.id));
				goName.append(")");
				//Draw Node. Trees will always return true if open, so use IsItemClicked to set object instead!
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			}
			

			//Use address of id as ptr_id
			ImGui::TreeNodeEx(&handle.id, nodeFlags, goName.c_str());

			if (isNameEmpty) {
				ImGui::PopStyleColor();
			}

			//Standard Click and ctrl click
			if (ImGui::IsItemClicked(0)) {
				//Check if handle has been selected
				bool hasBeenSelected = false;
				int counter = 0;
				for (counter = 0; counter < selected_gameObjects.size(); ++counter) {
					if (handle == selected_gameObjects[counter]) {
						hasBeenSelected = true;
						break;
					}
				}

				if (hasBeenSelected) {
					if (ImGui::IsKeyDown(static_cast<int>(Key::Control))) { //Deselect that particular handle
						selected_gameObjects.erase(selected_gameObjects.begin() + counter);

					}
					else { //Select as normal
						selected_gameObjects.clear();
						selected_gameObjects.push_back(handle);
					}

				}
				else {

					if (ImGui::IsKeyDown(static_cast<int>(Key::Control))) {
						selected_gameObjects.push_back(handle);

					}
					else {
						selected_gameObjects.clear();
						selected_gameObjects.push_back(handle);
					}
				}
			}



		});


		//for (auto& i : sceneGraph) {
		//	ImGui::PushID(i.obj.id);
		//
		//	string goName = "GameObject ";
		//	goName.append(std::to_string(i.obj.id));
		//	ImGui::Selectable(goName.c_str());
		//
		//	ImGui::PopID();
		//
		//}

		

	}

}
