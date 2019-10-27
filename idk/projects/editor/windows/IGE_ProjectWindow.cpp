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
#include <editor/DragDropTypes.h>
#include <core/GameObject.h>
#include <gfx/Texture.h>
#include <gfx/ShaderGraph.h>
#include <gfx/MaterialInstance.h>
#include <gfx/RenderTarget.h>
#include <prefab/PrefabUtility.h>
#include <prefab/Prefab.h>

#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#ifdef _WIN32 
#include <shellapi.h>
#endif

namespace idk {

	IGE_ProjectWindow::IGE_ProjectWindow()
		: IGE_IWindow{ "Project##ProjectWindow", true, ImVec2{ 800,200 }, ImVec2{ 200,200 } }
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
        if (!dir.IsDir())
            return;

        bool selected = dir == current_dir;
        if (!dirContainsDir(dir))
        {
            auto flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAllAvailWidth;
            if (selected) flags |= ImGuiTreeNodeFlags_Selected;
            ImGui::TreeNodeEx(dir.GetFileName().data(), flags);
            if (ImGui::IsItemClicked())
                current_dir = dir;
        }
        else
        {
            auto open = ImGui::TreeNodeEx(dir.GetFileName().data(),
                                          ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAllAvailWidth |
                                          ImGuiTreeNodeFlags_DefaultOpen | (selected ? ImGuiTreeNodeFlags_Selected : 0));
            if (ImGui::IsItemClicked())
                current_dir = dir;
            if (open)
            {
                for (const auto& path : dir.GetEntries())
                    displayDir(path);
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

    string IGE_ProjectWindow::unique_new_mount_path(string_view name, string_view ext)
    {
        string stripped_path{ current_dir.GetMountPath() };
        stripped_path += '/';
        stripped_path += name;

        string path = stripped_path;
        path += ext;

        int i = 0;
        while (PathHandle(path)) // already exists
        {
            path = stripped_path;
            path += std::to_string(++i);
            path += ext;
        }

        return path;
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
                if (ImGui::MenuItem("Folder"))
                {
                    auto folder_path = unique_new_mount_path("NewFolder", "");
                    fs::create_directory(Core::GetSystem<FileSystem>().GetFullPath(folder_path));
                    selected_path = folder_path;
                }
				if (ImGui::MenuItem("Material"))
				{
					auto path = unique_new_mount_path("NewMaterial", Material::ext);
					auto res = Core::GetResourceManager().Create<shadergraph::Graph>(path);
					if (res && *res)
						Core::GetResourceManager().Save(*res);
				}
				if (ImGui::MenuItem("Render Target"))
				{
					auto path = unique_new_mount_path("RenderTarget", RenderTarget::ext);
					auto res = Core::GetResourceManager().Create<RenderTarget>(path);
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
        displayDir(assets_dir);

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

        auto cursor_pos = ImGui::GetCursorPos();
        ImGui::Dummy(ImGui::GetContentRegionAvail());
        if (ImGui::BeginDragDropTarget())
        {
            if (const auto* payload = ImGui::AcceptDragDropPayload(DragDrop::GAME_OBJECT, ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                Handle<GameObject> go = *reinterpret_cast<Handle<GameObject>*>(payload->Data);
                PrefabUtility::SaveAndConnect(go, unique_new_mount_path(go->Name().size() ? go->Name() : "NewPrefab", Prefab::ext));
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::SetCursorPos(cursor_pos);


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
                ImVec4 tint = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
                ImVec4 selected_tint = ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered);

                if (path.IsDir())
                {
                    static auto folder_icon = *Core::GetResourceManager().Load<Texture>("/editor_data/icons/folder.png");
                    id = folder_icon->ID();
                }
                else
                {
                    auto handle = getOrLoadFirstAsset(path);
                    RscHandle<Texture> tex = std::visit([](auto h)
                    {
                        using T = typename decltype(h)::Resource;

                        if (!h)
                            return RscHandle<Texture>();
                        if constexpr (std::is_same_v<T, Texture>)
                            return h;
                        else if constexpr (std::is_same_v<T, Material> || std::is_same_v<T, shadergraph::Graph>)
                        {
                            static auto material_icon = *Core::GetResourceManager().Load<Texture>("/editor_data/icons/material.png");
                            return material_icon;
                        }
                        else if constexpr (std::is_same_v<T, MaterialInstance>)
                        {
                            static auto material_icon = *Core::GetResourceManager().Load<Texture>("/editor_data/icons/matinst.png");
                            return material_icon;
                        }
                        else
                            return RscHandle<Texture>();
                    }, handle);

                    if (tex)
                    {
                        id = tex->ID();
                        float aspect = tex->AspectRatio();
                        if (aspect > 1.0f)
                            sz.y /= aspect;
                        else if (aspect < 1.0f)
                            sz.x /= aspect;
                        if (handle.resource_id() == BaseResourceID<Texture>)
                        {
                            tint = /*ImVec4(0.9f, 0.9f, 0.9f, 1);*/
                                selected_tint = ImVec4(1, 1, 1, 1);
                        }
                    }
                }

                auto cursorpos = ImGui::GetCursorPos();
                auto offset = (ImVec2(icon_sz, icon_sz) - sz) * 0.5f;
                ImGui::SetCursorPos(cursorpos + offset);
                if (id)
                    ImGui::Image(id, sz, ImVec2(0, 0), ImVec2(1, 1), selected_path == path ? selected_tint : tint);
                else
                    ImGui::InvisibleButton("preview", sz);
                ImGui::SetCursorPos(cursorpos + ImVec2(0, icon_sz + ImGui::GetStyle().ItemSpacing.y));

                // todo: open arrow for bundle
            }

            ImVec2 text_frame_sz{ icon_sz, ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2 };

            if (selected_path == path)
            {
                static char buf[256];

                if (renaming_selected_asset)
                {
                    ImGui::SetNextItemWidth(icon_sz);
                    if (ImGui::InputText("##nolabel", buf, 256, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        name = buf;
                        renaming_selected_asset = false;

                        fs::path old_path = selected_path.GetFullPath();
                        fs::path new_path = Core::GetSystem<FileSystem>().GetFullPath(unique_new_mount_path(name, selected_path.GetExtension()));
                        fs::rename(old_path, new_path);

                        // move meta file as well
                        old_path += ".meta";
                        if (fs::exists(old_path))
                        {
                            new_path += ".meta";
                            fs::rename(old_path, new_path);
                        }
                    }
                    if (just_rename)
                    {
                        ImGui::SetKeyboardFocusHere(-1);
                        just_rename = false;
                    }
                    else if ((ImGui::IsItemDeactivated()))
                        renaming_selected_asset = false;
                }
                else
                {
                    auto cursor_y = ImGui::GetCursorPosY();

                    ImGui::GetWindowDrawList()->AddRectFilled(
                        ImGui::GetCursorScreenPos(),
                        ImGui::GetCursorScreenPos() + text_frame_sz,
                        ImGui::GetColorU32(ImGuiCol_FrameBgHovered),
                        text_frame_sz.y * 0.5f);

                    if (ImGui::InvisibleButton("rename_hitbox", text_frame_sz))
                    {
                        renaming_selected_asset = true;
                        just_rename = true;
                        strcpy_s(buf, name.c_str());
                    }

                    ImGui::SetCursorPosY(cursor_y + ImGui::GetStyle().FramePadding.y);

                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (icon_sz - label_sz.x) * 0.5f); // center text
                    ImGui::Text(label.c_str());
                }
            }
            else // not selected
            {
                auto cursor_y = ImGui::GetCursorPosY();
                ImGui::Dummy(text_frame_sz);
                ImGui::SetCursorPosY(cursor_y + ImGui::GetStyle().FramePadding.y);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (icon_sz - label_sz.x) * 0.5f); // center text
                ImGui::Text(label.c_str());
            }

            ImGui::PopID();
            ImGui::EndGroup();

            if (!path.IsDir())
            {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    auto get_res = Core::GetResourceManager().Get(path);
                    if (get_res && get_res->Count())
                    {
                        DragDrop::SetResourcePayload(*get_res);
                        ImGui::Text(path.GetMountPath().data());
                    }
                    ImGui::EndDragDropSource();
                }
            }
            else
            {
                if (ImGui::BeginDragDropTarget())
                {
                    if (ImGui::AcceptDragDropPayload(DragDrop::RESOURCE, ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
                    {
                        const auto& vec = DragDrop::GetResourcePayloadData();
                        auto res_path = std::visit([](auto h) { return Core::GetResourceManager().GetPath(h); }, vec[0]);
                        if (res_path)
                        {
                            PathHandle old_path_handle = *res_path;
                            fs::path old_path = old_path_handle.GetFullPath();

                            auto _curr_dir = current_dir;
                            current_dir = path; // unique_new_mount_path uses current_dir
                            fs::path new_path = Core::GetSystem<FileSystem>().GetFullPath(
                                unique_new_mount_path(old_path_handle.GetStem(), old_path_handle.GetExtension()));

                            fs::rename(old_path, new_path);

                            // move meta file as well
                            old_path += ".meta";
                            if (fs::exists(old_path))
                            {
                                new_path += ".meta";
                                fs::rename(old_path, new_path);
                            }

                            current_dir = _curr_dir;
                        }
                    }
                    auto get_res = Core::GetResourceManager().Get(path);
                    if (get_res && get_res->Count())
                    {
                        DragDrop::SetResourcePayload(*get_res);
                        ImGui::Text(path.GetMountPath().data());
                    }
                    ImGui::EndDragDropTarget();
                }
            }

            if (ImGui::IsItemClicked())
            {
                clicked_path = path;
            }

            if (!just_rename && clicked_path == path && ImGui::IsItemHovered() && ImGui::IsMouseReleased(0))
            {
                selected_path = path;
                renaming_selected_asset = false;
                if (!path.IsDir())
                {
                    auto get_res = Core::GetResourceManager().Get(path);
                    if (get_res && get_res->Count())
                    {
                        selected_assets.clear();
                        for (auto h : get_res->GetAll())
                            selected_assets.push_back(h);
                        OnAssetsSelected.Fire(span<GenericResourceHandle>(selected_assets));
                    }
                }
            }
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && !renaming_selected_asset)
            {
                if (path.IsDir())
                    current_dir = path;
                else
                    OnAssetDoubleClicked.Fire(selected_assets[0]);
            }

            if(ImGui::BeginPopupContextItem(path.GetMountPath().data()))
            {
                if (path.GetExtension() == shadergraph::Graph::ext)
                {
                    if (ImGui::MenuItem("Create Material Instance"))
                    {
                        string filename{ path.GetStem() };
                        filename += "_Inst";
                        auto create_path = unique_new_mount_path(filename, MaterialInstance::ext);
                        auto res = Core::GetResourceManager().Create<MaterialInstance>(create_path);
                        if (res && *res)
                        {
                            res.value()->material = *Core::GetResourceManager().Get<Material>(path);
                            Core::GetResourceManager().Save(*res);
                        }
                    }
                    ImGui::Separator();
                }

                if (ImGui::MenuItem("Show in Explorer"))
                {
#ifdef _WIN32 
                    auto args = "/select,\"" + string(fs::path(path.GetFullPath()).make_preferred().string()) + '"';
                    ShellExecuteA(NULL, "open", "explorer.exe", args.c_str(), NULL, SW_SHOWNORMAL);
#endif
                    ImGui::Separator();
                }

                //if (ImGui::MenuItem("Rename"))
                //{
                //    just_rename = true;
                //    renaming_selected_asset = true;
                //}
                if (ImGui::MenuItem("Delete"))
                {
                    fs::path full_path = path.GetFullPath();
                    fs::path meta_path = full_path;
                    meta_path += ".meta";

                    if (fs::exists(meta_path))
                        fs::remove(meta_path);
                    fs::remove(full_path);
                }

                ImGui::EndPopup();
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
        {
            string str = selected_path.GetMountPath().data() + 1;
            str[0] = 'A';
            ImGui::Text(str.c_str());
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();


	}

}
