

#include "pch.h"
#include "IGE_Window.h"
#include <iostream>

namespace idk {
	IGE_Window::IGE_Window(char const* wName, bool isWindowOpenByDefault, ImVec2 size, ImVec2 position, ImVec2 pivot)
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

	IGE_Window::~IGE_Window()
	{
	}

	void IGE_Window::BeginWindow() {
		ImGui::SetNextWindowPos(window_position, pos_condition_flags, window_pivot);
		ImGui::SetNextWindowSize(window_size, size_condition_flags);
		ImGui::Begin(window_name, &is_open, window_flags);
	}

	void IGE_Window::BeginWindow(ImGuiCond_ conditionFlags, ImGuiWindowFlags wFlags) {
		ImGui::SetNextWindowPos(window_position, conditionFlags, window_pivot);
		ImGui::SetNextWindowSize(window_size, conditionFlags);
		ImGui::Begin(window_name, &is_open, wFlags);
	}

	void IGE_Window::BeginWindow(ImGuiCond_ pcf, ImGuiCond_ scf, ImGuiWindowFlags wFlags) {
		ImGui::SetNextWindowPos(window_position, pcf, window_pivot);
		ImGui::SetNextWindowSize(window_size, scf);
		ImGui::Begin(window_name, &is_open, wFlags);
	}


	void IGE_Window::EndWindow() {

		//Check if mouse is at this window or not
		is_mouse_hovered	= ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
		is_window_collapsed = ImGui::IsWindowCollapsed();
		window_position		= ImGui::GetWindowPos();
		window_size			= ImGui::GetWindowSize();
		itemCounter			= 0;
		ImGui::End();
	}

	void IGE_Window::DrawHelpMarker(const char* desc) {
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

	void IGE_Window::DrawText(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);
		va_end(args);
	}

	void IGE_Window::DrawText(const std::string& fmt, ...) {
		va_list args;
		const char* temp = fmt.c_str();
		va_start(args, temp);
		ImGui::TextV(temp, args);
		va_end(args);
	}


	void IGE_Window::DrawTextColor(const ImVec4& color, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		ImGui::TextColoredV(color, fmt, args);
		va_end(args);

	}
	void IGE_Window::DrawTextColor(float r, float g, float b, float a, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		ImGui::TextColoredV(ImVec4{ r,g,b,a }, fmt, args);
		va_end(args);

	}

	void IGE_Window::DrawOnSameLine(float pos_x, float spacing_w) {
		ImGui::SameLine(pos_x, spacing_w);
	}

	void IGE_Window::DrawSpacing() {
		ImGui::Spacing();
	}

	bool IGE_Window::DrawButton(const char* label, const ImVec2& size) {
		bool i;
		ImGui::PushID(itemCounter++);
		i = ImGui::Button(label, size);
		ImGui::PopID();
		//std::cout << "itemcount" << itemCounter;
		return i;

	}

	void IGE_Window::DrawBorderRect(const ImVec2& minPos, const ImVec2& maxPos, const ImColor& color, const float& lineSize)
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

	void IGE_Window::RepositionWindow() {
		ImGui::SetWindowSize(window_name, start_window_size);
		ImGui::SetWindowPos(window_name, start_window_position);
	}

}