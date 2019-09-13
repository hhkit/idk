//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_ProjectWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		4 SEPT 2019
//@brief	

/*
This window controls the top main menu bar. It also controls the docking
of the editor.
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <editor/windows/IGE_ProjectWindow.h>
#include <editorstatic/imgui/imgui_internal.h> //InputTextEx
#include <app/Application.h>
#include <iostream>

namespace idk {

	IGE_ProjectWindow::IGE_ProjectWindow()
		:IGE_IWindow{ "Project##ProjectWindow",true,ImVec2{ 800,200 },ImVec2{ 200,200 } } {		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoCollapse;



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
		float startPos = ImGui::GetCursorPosY();
		if (ImGui::Button("Create")) {



		}

		ImGui::SetCursorPos(ImVec2{ window_size.x - 300,startPos });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
		static char searchBarChar[512];
		if (ImGui::InputTextEx("##ToolBarSearchBar",NULL, searchBarChar, 512, ImVec2{250,ImGui::GetFrameHeight()-2}, ImGuiInputTextFlags_None)) {
			//Do something
		}
		ImGui::PopStyleVar();


		ImGui::EndChild();

		//ImGui columns are annoying

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		ImGui::Columns(2);
		ImGui::PopStyleVar();


		float currentWidth = ImGui::GetColumnWidth(-1);
		currentWidth = currentWidth < 100 ? 100 : currentWidth;
		currentWidth = currentWidth > 400 ? 400 : currentWidth;
		ImGui::SetColumnWidth(-1, currentWidth);

		ImGui::SetCursorPos(ImVec2{ 0.0f,ImGui::GetFrameHeight() + toolBarSize.y });


		const ImVec2 assetViewer1Size{ 0, 0 };
		const ImGuiWindowFlags assetViewer1Flags = ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoCollapse;


		ImGui::BeginChild("AssetViewer1", assetViewer1Size, true, assetViewer1Flags);


		//if (ImGui::TreeNodeEx("Asset", ImGuiTreeNodeFlags_OpenOnDoubleClick)) {
		if (ImGui::TreeNodeEx("Asset")) {

			ImGui::TreePop();

		}


		ImGui::EndChild();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 5);
		ImGui::NextColumn();
		ImGui::PopStyleVar(2);

		const ImVec2 assetViewer2Size{ 0, 0 };
		const ImGuiWindowFlags assetViewer2Flags = ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoCollapse;

		ImGui::SetCursorPos(ImVec2{ assetViewer1Size.x,ImGui::GetFrameHeight() + toolBarSize.y });

		ImGui::BeginChild("AssetViewer2", assetViewer2Size, false, assetViewer2Flags);
		//ImGui::GetWindowSize();
		//Top Bar
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(cursorPos, ImVec2{ cursorPos.x + window_size.x +5,cursorPos.y + 18 }, ImGui::GetColorU32(ImGuiCol_TitleBg));


		ImGui::EndChild();


	}

}
