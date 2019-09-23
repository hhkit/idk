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
#include <editor/IDE.h>
#include <gfx/Texture.h>

#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

namespace idk {

	IGE_ProjectWindow::IGE_ProjectWindow()
		: IGE_IWindow{ "Project##ProjectWindow", true, ImVec2{ 800,200 }, ImVec2{ 200,200 } }
    {
        window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
        selected_dir = "/assets";
	}

	void IGE_ProjectWindow::BeginWindow()
	{
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{ 50.0f,100.0f });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
	}

    static bool dirContainsDir(PathHandle dir)
    {
        return dir.GetPaths(FS_FILTERS::DIR).size();
    }

    void IGE_ProjectWindow::displayDir(PathHandle dir)
    {
        for (const auto& path : dir.GetPaths())
        {
            if (!path.IsDir())
                continue;

            bool selected = path == selected_dir;
            if (!dirContainsDir(path))
            {
                auto flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAllAvailWidth;
                if (selected) flags |= ImGuiTreeNodeFlags_Selected;
                ImGui::TreeNodeEx(path.GetFileName().data(), flags);
                if (ImGui::IsItemClicked())
                    selected_dir = path;
            }
            else
            {
                auto open = ImGui::TreeNodeEx(path.GetFileName().data(),
                    ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAllAvailWidth |
                    ImGuiTreeNodeFlags_DefaultOpen | (selected ? ImGuiTreeNodeFlags_Selected : 0));
                if (ImGui::IsItemClicked())
                    selected_dir = path;
                displayDir(path);
                if (open)
                    ImGui::TreePop();
            }
        }
    }

	void IGE_ProjectWindow::Update()
	{
        ImGui::PopStyleVar(2);

		ImGuiStyle& style = ImGui::GetStyle();

		//ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_TitleBgActive]);

		//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		//Tool bar

		//ImGui::SetCursorPos(ImVec2{ 0.0f,ImGui::GetFrameHeight() });

        ImGui::BeginMenuBar();
        {
            if (ImGui::Button("Create"))
            {
            }

            static char searchBarChar[128];
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
            auto w = std::fminf(250.0f, window_size.x - ImGui::GetCursorPosX() - 50.0f);
            ImGui::SetCursorPosX(window_size.x - w - 50.0f);
            if (ImGui::InputTextEx("##ToolBarSearchBar", NULL, searchBarChar, 512, ImVec2{ w, ImGui::GetFrameHeight() - 2 }, ImGuiInputTextFlags_None))
            {
                //Do something
            }
            ImGui::PopStyleVar();
        }
        ImGui::EndMenuBar();

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


        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 4.0f));
		ImGui::BeginChild("AssetViewer1", ImVec2(), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
        ImGui::PopStyleVar();

        PathHandle assets_dir = "/assets";

        {
            bool selected = assets_dir == selected_dir;
            auto open = ImGui::TreeNodeEx("Assets",
                            ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow |
                            ImGuiTreeNodeFlags_DefaultOpen | (selected ? ImGuiTreeNodeFlags_Selected : 0));
            if (ImGui::IsItemClicked())
                selected_dir = assets_dir;
            if (open)
            {
                displayDir(assets_dir);
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
        auto menu_bar_col = ImGui::GetStyleColorVec4(ImGuiCol_MenuBarBg);
        menu_bar_col.w *= 0.5f;
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, menu_bar_col);
        ImGui::BeginChild("AssetViewer2", ImVec2(0, ImGui::GetContentRegionAvail().y - line_height - 4.0f), false,
                          ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysUseWindowPadding);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        ImGui::BeginMenuBar();
        {
            ImGui::PushStyleColor(ImGuiCol_Button, 0);

            if (ImGui::Button("Assets"))
                selected_dir = assets_dir;

            if (selected_dir != assets_dir)
            {
                string concat;
                int count = 0;
                for (const auto& part : fs::relative(selected_dir.GetFullPath(), assets_dir.GetFullPath()))
                {
                    auto str = part.string();
                    concat += '/';
                    concat += str;

                    ImGui::RenderArrowPointingAt(ImGui::GetWindowDrawList(),
                        ImGui::GetCursorScreenPos() + ImVec2(0, ImGui::GetTextLineHeight() / 2 + ImGui::GetStyle().FramePadding.y) + ImVec2(2.0f, 2.0f),
                        ImVec2(3.0f, 2.0f), ImGuiDir_Right, ImGui::GetColorU32(ImGuiCol_Text));
                    ImGui::Spacing();
                    ImGui::PushID(str.c_str());
                    if (ImGui::Button(str.c_str()))
                        selected_dir = "/assets" + concat;
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
        if (icons_per_row < selected_dir.GetPaths().size())
            spacing.x = (content_region.x - icons_per_row * icon_sz) / (icons_per_row + 1);

        ImGui::SetCursorPosX(spacing.x);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spacing.y);

        static bool renaming_selected_asset = false;

        int col = 0;
        for (const auto& path : selected_dir.GetPaths())
        {
            const auto ext = path.GetExtension();
            if (ext == ".meta")
                continue;

            string name{ path.GetFileName() };
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

            { // preview image / icon
                void* id = 0;
                vec2 sz{ icon_sz, icon_sz };
                if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".dds")
                {
                    auto files = Core::GetResourceManager().LoadFile(path);
                    if (files.resources.size())
                    {
                        auto tex = files[0].As<Texture>();
                        id = tex->ID();
                        float aspect = tex->AspectRatio();
                        if (aspect > 1.0f)
                            sz.y /= aspect;
                        else if (aspect < 1.0f)
                            sz.x /= aspect;
                    }
                }
                ImGui::Image(id, sz);
            }

            if (selected_asset == path)
            {
                static char buf[256];
                static bool first_focus = false;

                if (renaming_selected_asset)
                {
                    ImGui::SetNextItemWidth(icon_sz);
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2());
                    if (ImGui::InputText("##nolabel", buf, 256, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        name = buf;
                        renaming_selected_asset = false;
                    }
                    ImGui::PopStyleVar();
                    if (first_focus)
                    {
                        ImGui::SetKeyboardFocusHere(-1);
                        first_focus = false;
                    }
                    else if ((ImGui::IsItemDeactivated()))
                        renaming_selected_asset = false;
                }
                else if (!renaming_selected_asset)
                {
                    if (ImGui::InvisibleButton("rename_hitbox", ImVec2{ icon_sz, line_height }))
                    {
                        renaming_selected_asset = true;
                        first_focus = true;
                        strcpy_s(buf, name.c_str());
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

                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (icon_sz - label_sz.x) * 0.5f); // center text
                    ImGui::Text(label.c_str());
                }
            }
            else
            {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (icon_sz - label_sz.x) * 0.5f); // center text
                ImGui::Text(label.c_str());
            }

            ImGui::PopID();
            ImGui::EndGroup();

            if (ImGui::IsItemClicked())
            {
                selected_asset = path;
                renaming_selected_asset = false;
                OnAssetSelected.Fire(selected_asset);
            }
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && !renaming_selected_asset)
            {
                if (path.IsDir())
                    selected_dir = path;
                else
                    OnAssetDoubleClicked.Fire(selected_asset);
            }

            if (!path.IsDir())
            {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    ImGui::SetDragDropPayload(DragDrop::RESOURCE, &selected_asset, sizeof(string)); // "STRING" is a tag! This is used in IGE_InspectorWindow
                    ImGui::Text("Drag to inspector button.");
                    ImGui::Text(selected_asset.GetMountPath().data());
                    ImGui::EndDragDropSource();
                }
            }

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

        // footer
        ImGui::PushStyleColor(ImGuiCol_ChildBg, menu_bar_col);
        ImGui::BeginChild("footer", ImVec2(0, line_height));
        ImGui::SetCursorPosX(2.0f);
        if (selected_asset)
            ImGui::Text(selected_asset.GetMountPath().data());
        ImGui::EndChild();
        ImGui::PopStyleColor();


	}

}
