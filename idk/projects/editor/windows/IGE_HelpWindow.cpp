//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_HelpWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.co
/*

*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <editor/windows/IGE_HelpWindow.h>
namespace idk {


	IGE_HelpWindow::IGE_HelpWindow()
		:IGE_IWindow{ "Help Window",false,ImVec2{ 500,300 },ImVec2{ 200,200 } } {		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoDocking 
					 | ImGuiWindowFlags_NoCollapse;

		size_condition_flags = ImGuiCond_FirstUseEver;
		pos_condition_flags = ImGuiCond_FirstUseEver;
		
	}


	void IGE_HelpWindow::Update() {

		ImGui::Text("SHORTCUTS (V1.0)");
		const ImVec4 color = { 0,0.7f,0.1f,1.0f };
		auto cursorPos = ImGui::GetCursorPos();
		const auto XOffset = 100.0f;
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		for (auto i : paired_controls_1) {
			ImGui::Text(i.first);
		}
		ImGui::PopStyleColor();
		ImGui::SetCursorPosY(cursorPos.y);
		for (auto i : paired_controls_1) {
			ImGui::SetCursorPosX(XOffset);
			ImGui::Text(i.second);
		}

		ImGui::Separator();
		cursorPos = ImGui::GetCursorPos();
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		for (auto i : paired_controls_2) {
			ImGui::Text(i.first);
		}
		ImGui::PopStyleColor();
		ImGui::SetCursorPosY(cursorPos.y);
		for (auto i : paired_controls_2) {
			ImGui::SetCursorPosX(XOffset*2);

			ImGui::Text(i.second);
		}

	}






}
