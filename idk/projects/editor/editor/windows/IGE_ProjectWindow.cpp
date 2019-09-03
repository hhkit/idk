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
#include <editor/windows/IGE_ProjectWindow.h>
#include <app/Application.h>
#include <iostream>

namespace idk {

	IGE_ProjectWindow::IGE_ProjectWindow()
		:IGE_IWindow{ "Project##ProjectWindow",true,ImVec2{ 800,200 },ImVec2{ 200,200 } } {		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoCollapse
					 | ImGuiWindowFlags_NoSavedSettings;

		//size_condition_flags = ImGuiCond_Always;
		//pos_condition_flags = ImGuiCond_Always;



	}

	void IGE_ProjectWindow::BeginWindow()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{50.0f,100.0f});

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));


	}
	void IGE_ProjectWindow::Update()
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
		ImGui::BeginChild("ProjectToolBar", toolBarSize, false, childFlags);
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(3);

		ImGui::SetCursorPosX(5);

		if (ImGui::Button("Create")) {



		}


		ImGui::EndChild();

		//ImGui columns are annoying
		//ImGui::Columns(2);
		//float currentWidth = ImGui::GetColumnWidth(-1);
		//currentWidth = currentWidth < 100 ? 100 : currentWidth;
		//currentWidth = currentWidth > 400 ? 400 : currentWidth;
		//ImGui::SetColumnWidth(-1, currentWidth);
		//ImGui::NextColumn();

		ImGui::SetCursorPos(ImVec2{ 0.0f,ImGui::GetFrameHeight() + toolBarSize.y });


		const ImVec2 assetViewerSize{ 150, 0 };
		const ImGuiWindowFlags assetViewerFlags = ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoCollapse;


		ImGui::BeginChild("AssetViewer", assetViewerSize, true, assetViewerFlags);


		//if (ImGui::TreeNodeEx("Asset", ImGuiTreeNodeFlags_OpenOnDoubleClick)) {
		if (ImGui::TreeNodeEx("Asset")) {

			ImGui::TreePop();

		}


		ImGui::EndChild();


	}

}
