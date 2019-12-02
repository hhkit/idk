//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_IWindow.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		30 AUG 2019
//@brief	

/*
//This is an interface parent class to create other Windows to the editor
//You can also use imgui functions instead of using the inherited functions to create your own custom style.
//This is still work in progress

//When buidling window classes, public parent this class.
//You can override virtual functions to the child class to do something before or after.
//An example is shown in IGE_DebugWindow.cpp

//You cannot have windows of the same name in other classes. You can however unique identify them by adding ## in its windowName initializer

*/
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <editor/windows/IGE_IWindow.h>
#include <iostream>

namespace idk {
	IGE_IWindow::IGE_IWindow(char const* wName, bool isWindowOpenByDefault, ImVec2 size, ImVec2 position, ImVec2 pivot)
		: window_name			{ wName }
		, is_open				{ isWindowOpenByDefault }
		, window_size			{ size }
		, window_position		{ position }
		, window_pivot			{ pivot }
		, start_window_position	{ position }
		, start_window_size		{ size }
		, itemCounter			{ 0 }
	{
	}

	IGE_IWindow::~IGE_IWindow()
	{
	}




	void IGE_IWindow::DrawHelpMarker(const char* desc) {
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	void IGE_IWindow::DrawToolTipOnHover(const char* desc)
	{
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	void IGE_IWindow::DrawText(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);
		va_end(args);
	}

	void IGE_IWindow::DrawText(const std::string& fmt, ...) {
		va_list args;
		const char* temp = fmt.c_str();
		va_start(args, temp);
		ImGui::TextV(temp, args);
		va_end(args);
	}


	void IGE_IWindow::DrawTextColor(const ImVec4& color, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		ImGui::TextColoredV(color, fmt, args);
		va_end(args);

	}
	void IGE_IWindow::DrawTextColor(float r, float g, float b, float a, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		ImGui::TextColoredV(ImVec4{ r,g,b,a }, fmt, args);
		va_end(args);

	}

	void IGE_IWindow::DrawOnSameLine(float pos_x, float spacing_w) {
		ImGui::SameLine(pos_x, spacing_w);
	}

	void IGE_IWindow::DrawSpacing() {
		ImGui::Spacing();
	}

	bool IGE_IWindow::DrawButton(const char* label, const ImVec2& size) {
		bool i;
		ImGui::PushID(itemCounter++);
		i = ImGui::Button(label, size);
		ImGui::PopID();
		//std::cout << "itemcount" << itemCounter;
		return i;

	}

	void IGE_IWindow::DrawBorderRect(const ImVec2& minPos, const ImVec2& maxPos, const ImColor& color, const float& lineSize)
	{
		// draw grid
		auto draw_list = ImGui::GetWindowDrawList();
		// draw horizontal line
		draw_list->AddLine(
			minPos,
			ImVec2(maxPos.x, minPos.y),
			color,
			lineSize
		);
		draw_list->AddLine(
			ImVec2(minPos.x, maxPos.y),
			maxPos,
			color,
			lineSize
		);
		// draw vertical line
		draw_list->AddLine(
			minPos,
			ImVec2(minPos.x, maxPos.y),
			color,
			lineSize
		);
		draw_list->AddLine(
			ImVec2(maxPos.x, minPos.y),
			maxPos,
			color,
			lineSize
		);
	}

	void IGE_IWindow::BeginWindow_V()
	{

		BeginWindow();

		ImGui::SetNextWindowPos(window_position, pos_condition_flags, window_pivot);
		ImGui::SetNextWindowSize(window_size, size_condition_flags);
		is_window_displayed = ImGui::Begin(window_name, &is_open, window_flags);
	}

	void IGE_IWindow::EndWindow_V()
	{
		EndWindow();

		//Check if mouse is at this window or not
		is_mouse_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
		is_window_collapsed = ImGui::IsWindowCollapsed();
		window_position = ImGui::GetWindowPos();
		window_size = ImGui::GetWindowSize();
		itemCounter = 0;

		ImGui::End();
	}

	void IGE_IWindow::ResetWindowPosSize() {
		ImGui::SetWindowSize(window_name, start_window_size);
		ImGui::SetWindowPos(window_name, start_window_position);
	}

	void IGE_IWindow::DrawWindow()
	{
		if (!is_open) {
			return;
		}
		BeginWindow_V();

		Update();

		EndWindow_V();

	}

}