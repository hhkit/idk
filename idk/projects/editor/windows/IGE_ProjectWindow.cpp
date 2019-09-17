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
		: IGE_IWindow{ "Project##ProjectWindow", true, ImVec2{ 800,200 }, ImVec2{ 200,200 } }
    {
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
        if(ImGui::IsWindowAppearing())
            ImGui::SetColumnWidth(-1, 200);

		ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin());


        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 2.0f));
		ImGui::BeginChild("AssetViewer1", ImVec2(), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
        ImGui::PopStyleVar();

        fs::path assets_dir = Core::GetSystem<FileSystem>().GetAssetDir();

        {
            bool selected = assets_dir == selected_dir;
            auto open = ImGui::TreeNodeEx("Assets",
                            ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow |
                            ImGuiTreeNodeFlags_DefaultOpen | (selected ? ImGuiTreeNodeFlags_Selected : 0));
            if (ImGui::IsItemClicked())
                selected_dir = assets_dir.string();
            if (open)
            {
                displayDir(assets_dir.string());
                ImGui::TreePop();
            }
        }

		ImGui::EndChild();


		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 5);
		ImGui::NextColumn();
		ImGui::PopStyleVar(2);

        const auto line_height = ImGui::GetTextLineHeight();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 2.0f));
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, 0xff333333);
        ImGui::BeginChild("AssetViewer2", ImVec2(0, ImGui::GetContentRegionAvail().y - line_height - 4.0f), false,
                          ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysUseWindowPadding);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

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
        
        const auto content_region = ImGui::GetContentRegionAvail();
        const float icon_sz = 64.0f;
        auto spacing = ImGui::GetStyle().ItemSpacing;
        const int icons_per_row = static_cast<int>((content_region.x - spacing.x) / (icon_sz + spacing.x));

        // if more icons than what can fit in a row, justify it
        if (icons_per_row < std::distance(fs::directory_iterator(selected_dir), fs::directory_iterator()))
            spacing.x = (content_region.x - icons_per_row * icon_sz) / (icons_per_row + 1);

        ImGui::SetCursorPosX(spacing.x);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spacing.y);

        static bool renaming_selected_asset = false;

        int col = 0;
        for (const auto& entry : fs::directory_iterator(selected_dir))
        {
            const auto& path = entry.path();
            const auto ext = path.extension().string();
            if (ext == ".meta")
                continue;

            auto name = path.filename().string();
            auto label = name;
            auto label_sz = ImGui::CalcTextSize(label.c_str());
            while (label_sz.x > icon_sz)
            {
                // hello world => hello w...
                label.pop_back(); label.pop_back(); label.pop_back(); label.pop_back();
                label += "...";
                label_sz = ImGui::CalcTextSize(label.c_str());
            }

            ImGui::BeginGroup();
            ImGui::PushID(name.c_str());

            if (ImGui::Button("test", ImVec2(icon_sz, icon_sz)))
            {
                if (entry.is_directory())
                {
                    selected_dir = path.string();
                }
                else
                {
                    selected_asset = path.string();
                }
            }
			if (ImGui::IsItemClicked()) { //Activates on click i.e: it is now single click for assets, but double click for directory
				if (!entry.is_directory())
				{
					selected_asset = path.string();
				}
			}
			if (!entry.is_directory()) {

				if (ImGui::BeginDragDropSource()) {
					ImGui::SetDragDropPayload(reflect::get_type<string>().name().data(), &selected_asset, sizeof(string)); // "STRING" is a tag! This is used in IGE_InspectorWindow
					ImGui::Text("Drag to inspector button.");
					ImGui::Text(selected_asset.c_str());
					ImGui::EndDragDropSource();
				}
			}


            if (selected_asset == path)
            {
                if (ImGui::InvisibleButton("rename_hitbox", ImVec2{ icon_sz, line_height }))
                {
                    //renaming_selected_asset = true;
                }

                if (renaming_selected_asset)
                {
                    static char buf[256];
                    strcpy_s(buf, name.c_str());
                    ImGui::SetNextItemWidth(icon_sz);
                    if (ImGui::InputText("##nolabel", buf, 256, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        name = buf;
                        renaming_selected_asset = false;
                    }
                    if (ImGui::IsItemDeactivated() || !ImGui::IsItemFocused())
                        renaming_selected_asset = false;
                }
                else
                {
                    ImGui::GetWindowDrawList()->AddRectFilled(
                        ImGui::GetItemRectMin(),
                        ImGui::GetItemRectMax(),
                        ImGui::GetColorU32(ImGuiCol_FrameBg),
                        line_height * 0.5f);
                }
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - line_height - spacing.y);
            }

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (icon_sz - label_sz.x) * 0.5f); // center text
            ImGui::Text(label.c_str());

            ImGui::PopID();
            ImGui::EndGroup();

            if (++col == icons_per_row)
            {
                col = 0;
                ImGui::SetCursorPosX(spacing.x);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spacing.y);
            }
            else
            {
                ImGui::SameLine(0, spacing.x);
            }
        }

		ImGui::EndChild();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, 0xff333333);
        ImGui::BeginChild("footer", ImVec2(0, line_height));
        ImGui::SetCursorPosX(2.0f);
        if (!selected_asset.empty())
            ImGui::Text(("Assets" / fs::relative(selected_asset, assets_dir)).string().c_str());
        ImGui::EndChild();
        ImGui::PopStyleColor();


	}

}
