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
#include <gfx/ShaderGraph.h>

#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

namespace idk {

	IGE_ProjectWindow::IGE_ProjectWindow()
		: IGE_IWindow{ "Project##ProjectWindow", true, ImVec2{ 800,200 }, ImVec2{ 200,200 } }, selected_asset{ RscHandle<Texture>() }
    {
        window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
        current_dir = "/assets";
	}

	void IGE_ProjectWindow::BeginWindow()
	{
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{ 50.0f,100.0f });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
	}

    static bool dirContainsDir(PathHandle dir)
    {
        return dir.GetEntries(FS_FILTERS::DIR).size();
    }

    void IGE_ProjectWindow::displayDir(PathHandle dir)
    {
        for (const auto& path : dir.GetEntries())
        {
            if (!path.IsDir())
                continue;

            bool selected = path == current_dir;
            if (!dirContainsDir(path))
            {
                auto flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAllAvailWidth;
                if (selected) flags |= ImGuiTreeNodeFlags_Selected;
                ImGui::TreeNodeEx(path.GetFileName().data(), flags);
                if (ImGui::IsItemClicked())
                    current_dir = path;
            }
            else
            {
                auto open = ImGui::TreeNodeEx(path.GetFileName().data(),
                    ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAllAvailWidth |
                    ImGuiTreeNodeFlags_DefaultOpen | (selected ? ImGuiTreeNodeFlags_Selected : 0));
                if (ImGui::IsItemClicked())
                    current_dir = path;
                displayDir(path);
                if (open)
                    ImGui::TreePop();
            }
        }
    }

    GenericResourceHandle IGE_ProjectWindow::getOrLoadFirstAsset(PathHandle path)
    {
        auto get_res = Core::GetResourceManager().Get(path);
        if (get_res && get_res->Count())
            return get_res->GetAll()[0];
        else if (!get_res)
        {
            auto load_res = Core::GetResourceManager().Load(path);
            if (load_res && load_res->Count())
                return load_res->GetAll()[0];
        }
        
        return RscHandle<Texture>();
    }

	void IGE_ProjectWindow::Update()
	{
        ImGui::PopStyleVar(2);


        ImGui::BeginMenuBar();
        {
            if (ImGui::Button("Create"))
            {
                ImGui::OpenPopup("create_menu");
            }
            if (ImGui::BeginPopup("create_menu"))
            {
                if (ImGui::MenuItem("Material"))
                {
                    auto path = string{ current_dir.GetMountPath() } + "/NewMaterial.mat";
                    int i = 0;
                    while (PathHandle(path)) // already exists
                    {
                        path = string{ current_dir.GetMountPath() } + "NewMaterial";
                        path += std::to_string(++i);
                        path += ".mat";
                    }
                    auto res = Core::GetResourceManager().Create<shadergraph::Graph>(path);
                    if (res && *res)
                        Core::GetResourceManager().Save(*res);
                }
                ImGui::EndPopup();
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
            bool selected = assets_dir == current_dir;
            auto open = ImGui::TreeNodeEx("Assets",
                            ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow |
                            ImGuiTreeNodeFlags_DefaultOpen | (selected ? ImGuiTreeNodeFlags_Selected : 0));
            if (ImGui::IsItemClicked())
                current_dir = assets_dir;
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
                current_dir = assets_dir;

            if (current_dir != assets_dir)
            {
                string concat;
                int count = 0;
                for (const auto& part : fs::relative(current_dir.GetFullPath(), assets_dir.GetFullPath()))
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
                        current_dir = "/assets" + concat;
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
        if (icons_per_row < current_dir.GetEntries().size())
            spacing.x = (content_region.x - icons_per_row * icon_sz) / (icons_per_row + 1);

        ImGui::SetCursorPosX(spacing.x);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spacing.y);

        static bool renaming_selected_asset = false;

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

        int col = 0;
        for (const auto& path : current_dir.GetEntries())
        {
            const auto ext = path.GetExtension();
            if (ext == ".meta")
                continue;

            auto stem = path.GetFileName();
            stem.remove_suffix(path.GetExtension().size());
            string name{ stem };
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
                ImVec4 tint = ImColor(65, 153, 163).Value;
                ImVec4 selected_tint = ImColor(30, 120, 130).Value;

                if (path.IsDir())
                {
                    static auto folder_icon = *Core::GetResourceManager().Load<Texture>("/editor_data/icons/folder.png");
                    id = folder_icon->ID();
                }
                else
                {
                    RscHandle<Texture> tex = std::visit([](auto h)
                    {
                        using T = typename decltype(h)::Resource;

                        if (!h)
                            return RscHandle<Texture>();
                        if constexpr (std::is_same_v<T, Texture>)
                            return h;
                        else
                            return RscHandle<Texture>();
                    }, getOrLoadFirstAsset(path));

                    if (tex)
                    {
                        id = tex->ID();
                        float aspect = tex->AspectRatio();
                        if (aspect > 1.0f)
                            sz.y /= aspect;
                        else if (aspect < 1.0f)
                            sz.x /= aspect;
                        tint = ImVec4(1, 1, 1, 1);
                        selected_tint = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
                    }
                }

                if (id)
                    ImGui::Image(id, sz, ImVec2(0, 0), ImVec2(1, 1), selected_path == path ? selected_tint : tint);
                else
                    ImGui::InvisibleButton("preview", sz);

                // todo: open arrow for bundle
            }

            if (selected_path == path)
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

                        // dont work yet
                        //string str{ selected_path.GetParentMountPath() };
                        //str += '/';
                        //str += name;
                        //Core::GetResourceManager().Rename(selected_path, str);
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
                    //auto cursor_y = ImGui::GetCursorPosY();

                    //if (ImGui::InvisibleButton("rename_hitbox", ImVec2{ icon_sz, line_height }))
                    //{
                    //    renaming_selected_asset = true;
                    //    first_focus = true;
                    //    strcpy_s(buf, name.c_str());
                    //}
                    //else
                    //{
                        ImGui::GetWindowDrawList()->AddRectFilled(
                            ImGui::GetCursorScreenPos(),
                            ImGui::GetCursorScreenPos() + ImVec2(icon_sz, line_height),
                            ImGui::GetColorU32(ImGuiCol_FrameBg),
                            line_height * 0.5f);
                    //}

                    //ImGui::SetCursorPosY(cursor_y);

                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (icon_sz - label_sz.x) * 0.5f); // center text
                    ImGui::Text(label.c_str());
                }
            }
            else // not selected
            {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (icon_sz - label_sz.x) * 0.5f); // center text
                ImGui::Text(label.c_str());
            }

            ImGui::PopID();
            ImGui::EndGroup();

            if (!path.IsDir())
            {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    DragDrop::SetResourcePayload(getOrLoadFirstAsset(path));
                    ImGui::Text("Drag to inspector button.");
                    ImGui::Text(path.GetMountPath().data());
                    ImGui::EndDragDropSource();
                }
            }

            if (ImGui::IsItemClicked())
            {
                clicked_path = path;
            }

            if (clicked_path == path && ImGui::IsItemHovered() && ImGui::IsMouseReleased(0))
            {
                selected_path = path;
                renaming_selected_asset = false;
                if (!path.IsDir())
                {
                    selected_asset = getOrLoadFirstAsset(selected_path);
                    OnAssetSelected.Fire(selected_asset);
                }
            }
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && !renaming_selected_asset)
            {
                if (path.IsDir())
                    current_dir = path;
                else
                    OnAssetDoubleClicked.Fire(selected_asset);
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

        } // for each paths in dir

        ImGui::PopFont();

		ImGui::EndChild();

        // footer
        ImGui::PushStyleColor(ImGuiCol_ChildBg, menu_bar_col);
        ImGui::BeginChild("footer", ImVec2(0, line_height));
        ImGui::SetCursorPosX(2.0f);
        if (selected_path)
            ImGui::Text(selected_path.GetMountPath().data());
        ImGui::EndChild();
        ImGui::PopStyleColor();


	}

}
