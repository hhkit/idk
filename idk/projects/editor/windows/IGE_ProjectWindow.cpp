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

#include <filesystem>
namespace fs = std::filesystem;

namespace idk {

	IGE_ProjectWindow::IGE_ProjectWindow()
		:IGE_IWindow{ "Project##ProjectWindow",true,ImVec2{ 800,200 },ImVec2{ 200,200 } } {		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
        window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;

        selected_dir = Core::GetSystem<FileSystem>().GetAssetDir();
	}

	void IGE_ProjectWindow::BeginWindow()
	{
        ImGui::SetNextWindowSizeConstraints(ImVec2{ 50.0f,100.0f }, ImVec2());
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
	}

    static bool dirContainsDir(const fs::path& dir)
    {
        for (const auto& entry : fs::directory_iterator(dir))
        {
            if (entry.is_directory())
                return true;
        }
        return false;
    }

    void IGE_ProjectWindow::displayDir(const string& dir)
    {
        for (const auto& entry : fs::directory_iterator(dir))
        {
            if (!entry.is_directory())
                continue;

            const auto& path = entry.path();
            bool selected = path == selected_dir;
            if (!dirContainsDir(path))
            {
                auto flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                if (selected) flags |= ImGuiTreeNodeFlags_Selected;
                ImGui::TreeNodeEx(path.filename().string().c_str(), flags);
                if (ImGui::IsItemClicked())
                    selected_dir = path.string();
            }
            else
            {
                auto open = ImGui::TreeNodeEx(path.filename().string().c_str(),
                    ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow |
                    ImGuiTreeNodeFlags_DefaultOpen | (selected ? ImGuiTreeNodeFlags_Selected : 0));
                if (ImGui::IsItemClicked())
                    selected_dir = path.string();
                displayDir(path.string());
                if (open)
                    ImGui::TreePop();
            }
        }
    }

	void IGE_ProjectWindow::Update()
	{
        ImGui::PopStyleVar();

		ImGuiStyle& style = ImGui::GetStyle();

		//ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_TitleBgActive]);

		//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		//Tool bar

		//ImGui::SetCursorPos(ImVec2{ 0.0f,ImGui::GetFrameHeight() });

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::Button("Create"))
            {
            }
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
            static char searchBarChar[512];
            ImGui::SetCursorPosX(window_size.x - 300);
            if (ImGui::InputTextEx("##ToolBarSearchBar", NULL, searchBarChar, 512, ImVec2{ 250,ImGui::GetFrameHeight() - 2 }, ImGuiInputTextFlags_None))
            {
                //Do something
            }
            ImGui::PopStyleVar();
            ImGui::EndMenuBar();
        }

		//ImGui columns are annoying
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 2.0f));
		ImGui::Columns(2);
		ImGui::PopStyleVar();


		//float currentWidth = ImGui::GetColumnWidth(-1);
		//currentWidth = currentWidth < 100 ? 100 : currentWidth;
		//currentWidth = currentWidth > 400 ? 400 : currentWidth;
		//ImGui::SetColumnWidth(-1, currentWidth);

		ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin());


		ImGui::BeginChild("AssetViewer1");

        fs::path assets_dir = Core::GetSystem<FileSystem>().GetAssetDir();

		//if (ImGui::TreeNodeEx("Asset", ImGuiTreeNodeFlags_OpenOnDoubleClick)) {

        bool selected = assets_dir == selected_dir;
		if (ImGui::TreeNodeEx("Assets",
            ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_DefaultOpen | (selected ? ImGuiTreeNodeFlags_Selected : 0)))
        {
            displayDir(assets_dir.string());
			ImGui::TreePop();
		}

		ImGui::EndChild();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 5);
		ImGui::NextColumn();
		ImGui::PopStyleVar(2);

        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, 0xff333333);
        ImGui::BeginChild("AssetViewer2", ImVec2(), false, ImGuiWindowFlags_MenuBar);
        ImGui::PopStyleColor();

        if (ImGui::BeginMenuBar())
        {
            ImGui::PushStyleColor(ImGuiCol_Button, 0);

            if (ImGui::Button("Assets"))
                selected_dir = assets_dir.string();

            if (selected_dir != assets_dir)
            {
                fs::path concat;
                int count = 0;
                for (const auto& part : fs::relative(selected_dir, assets_dir))
                {
                    concat /= part;
                    auto str = part.string();
                    ImGui::RenderArrowPointingAt(ImGui::GetWindowDrawList(),
                        ImGui::GetCursorScreenPos() + ImVec2(0, ImGui::GetTextLineHeight() / 2 + ImGui::GetStyle().FramePadding.y) + ImVec2(2.0f, 2.0f),
                        ImVec2(3.0f, 2.0f), ImGuiDir_Right, ImGui::GetColorU32(ImGuiCol_Text));
                    ImGui::Spacing();
                    ImGui::PushID(str.c_str());
                    if (ImGui::Button(str.c_str()))
                        selected_dir = (fs::path(assets_dir) / concat).string();
                    ++count;
                }
                while (count--)
                    ImGui::PopID();
            }

            ImGui::PopStyleColor();

            ImGui::EndMenuBar();
        }
        


		ImGui::EndChild();


	}

}
