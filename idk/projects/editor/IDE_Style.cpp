#include "pch.h"
#include "IDE.h"
#include <file/FileSystem.h>

namespace idk
{

	void IDE::ApplyDefaultStyle()
	{
		auto& style = ImGui::GetStyle();
		style.FramePadding = ImVec2(4.0f, 0);
		style.ItemSpacing = ImVec2(6.0f, 3.0f);
		style.ItemSpacing = ImVec2(6.0f, 3.0f);
		style.WindowRounding = 0;
		style.TabRounding = 0;
		style.IndentSpacing = 14.0f;
		style.ScrollbarRounding = 0;
		style.GrabRounding = 0;
		style.ChildRounding = 0;
		style.PopupRounding = 0;
		style.FrameRounding = 1.0f;
		style.CurveTessellationTol = 0.5f;
	}

	void IDE::ApplyDefaultColors()
	{
		auto& style = ImGui::GetStyle();
		auto* colors = style.Colors;
		ImGui::StyleColorsDark();

		colors[ImGuiCol_CheckMark] = colors[ImGuiCol_Text];

		// grays
		colors[ImGuiCol_WindowBg] =
			ImColor(29, 34, 41).Value;
		colors[ImGuiCol_PopupBg] =
			colors[ImGuiCol_ScrollbarBg] =
			ImColor(43, 49, 56, 240).Value;
		colors[ImGuiCol_Border] =
			colors[ImGuiCol_Separator] =
			colors[ImGuiCol_ScrollbarGrab] =
			ImColor(63, 70, 77).Value;
		//ImColor(106, 118, 129).Value;

		colors[ImGuiCol_MenuBarBg] =
			ImColor(36, 58, 74).Value;
		colors[ImGuiCol_ScrollbarBg].w = 0.5f;

		// main accent - 2
		colors[ImGuiCol_TitleBg] =
			ImColor(5, 30, 51).Value;

		// main accent - 1
		colors[ImGuiCol_TitleBgActive] =
			colors[ImGuiCol_TabUnfocused] =
			ImColor(11, 54, 79).Value;

		// main accent
		colors[ImGuiCol_Tab] =
			colors[ImGuiCol_TabUnfocusedActive] =
			colors[ImGuiCol_FrameBg] =
			colors[ImGuiCol_Button] =
			colors[ImGuiCol_Header] =
			colors[ImGuiCol_SeparatorHovered] =
			colors[ImGuiCol_ScrollbarGrabHovered] =
			ImColor(23, 75, 111).Value;

		// main accent + 1
		colors[ImGuiCol_TabHovered] =
			colors[ImGuiCol_TabActive] =
			colors[ImGuiCol_ButtonHovered] =
			colors[ImGuiCol_FrameBgHovered] =
			colors[ImGuiCol_HeaderHovered] =
			colors[ImGuiCol_SeparatorActive] =
			colors[ImGuiCol_ScrollbarGrabActive] =
			ImColor(46, 115, 143).Value;

		// main accent + 2
		colors[ImGuiCol_TextSelectedBg] =
			colors[ImGuiCol_ButtonActive] =
			colors[ImGuiCol_FrameBgActive] =
			colors[ImGuiCol_HeaderActive] =
			ImColor(65, 153, 163).Value;

		// complement accent
		colors[ImGuiCol_PlotLinesHovered] =
			ImColor(222, 116, 35).Value;
		//style.Colors[ImGuiCol_PlotLinesHovered]
		// ImColor(

	}

	void IDE::LoadEditorFonts()
	{
		auto& fs = Core::GetSystem<FileSystem>();
		auto& io = ImGui::GetIO();

		ImFontConfig config;
		config.OversampleH = 5;
		config.OversampleV = 3;
		config.RasterizerMultiply = 1.5f;
		auto fontpath = fs.GetFullPath("/editor_data/fonts/SourceSansPro-Regular.ttf");
		auto fontpathbold = fs.GetFullPath("/editor_data/fonts/SourceSansPro-SemiBold.ttf");
		auto iconfontpath = fs.GetFullPath("/editor_data/fonts/" FONT_ICON_FILE_NAME_FAR);
		auto iconfontpath2 = fs.GetFullPath("/editor_data/fonts/" FONT_ICON_FILE_NAME_FAS);
		io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 15.0f, &config); // Default

		// merge in icons from MDI
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true; icons_config.GlyphMinAdvanceX = 14.0f; icons_config.GlyphOffset.y = 1.0f;
		io.Fonts->AddFontFromFileTTF(iconfontpath.c_str(), 14.0f, &icons_config, icons_ranges);
		io.Fonts->AddFontFromFileTTF(iconfontpath2.c_str(), 14.0f, &icons_config, icons_ranges);

		// smaller and bold style of default font
		io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 13.0f, &config); // Smaller
		io.Fonts->AddFontFromFileTTF(iconfontpath.c_str(), 12.0f, &icons_config, icons_ranges);
		io.Fonts->AddFontFromFileTTF(iconfontpath2.c_str(), 12.0f, &icons_config, icons_ranges);

		io.Fonts->AddFontFromFileTTF(fontpathbold.c_str(), 15.0f, &config); // Bold
		io.Fonts->AddFontFromFileTTF(iconfontpath.c_str(), 14.0f, &icons_config, icons_ranges);
		io.Fonts->AddFontFromFileTTF(iconfontpath2.c_str(), 14.0f, &icons_config, icons_ranges);

		io.Fonts->Build();
	}

}