#include "pch.h"
#include "IGE_ProjectSettings.h"
#include <core/GameObject.h>
#include <IncludeSystems.h>
#include <editor/utils.h>
#include <editor/imguidk.h>
#include <imgui/imgui_internal.h>

namespace idk
{

    IGE_ProjectSettings::IGE_ProjectSettings()
        : IGE_IWindow{ "Project Settings", false, ImVec2(500.0f, 750.0f) }
    {
        window_flags = 0;
    }

    void IGE_ProjectSettings::BeginWindow()
    {
    }



    template<typename T>
    static void DisplayConfig()
    {
    }

    template<>
    void DisplayConfig<TagManager>()
    {
        auto& sys = Core::GetSystem<TagManager>();
        auto config = sys.GetConfig();

        const float item_width = ImGui::GetWindowContentRegionWidth() * 0.6f;
        const float pad_y = ImGui::GetStyle().FramePadding.y;
        const float btn_width = ImGui::GetTextLineHeight() + pad_y * 2;
        bool changed = false;

        ImGui::PushItemWidth(item_width - btn_width - ImGui::GetStyle().ItemSpacing.x);

        ImGui::PushID("tags");
        if (ImGui::TreeNodeEx("Tags", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAllAvailWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen))
        {
            for (int i = 0; i < config.tags.size(); ++i)
            {
                const float cursor_y = ImGui::GetCursorPosY();
                ImGui::SetCursorPosY(cursor_y + pad_y);
                ImGui::Text("Tag %d", i + 1);

                ImGui::SetCursorPosY(cursor_y);
                ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);

                ImGui::PushID(i);

                char buf[32];
                strcpy_s(buf, config.tags[i].data());

                ImGui::InputText("", buf, 32);
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    config.tags[i] = buf;
                    changed = true;
                }

                ImGui::SameLine();
                if (ImGui::Button("-", ImVec2(btn_width, 0)))
                {
                    // remove and shift tags in the scene
                    for (auto& c : GameState::GetGameState().GetObjectsOfType<Tag>())
                    {
                        if (c.index == i + 1)
                            c.GetGameObject()->RemoveComponent(c.GetHandle());
                        else if (c.index > i + 1)
                            --c.index;
                    }

                    config.tags.erase(config.tags.begin() + i);
                    --i;
                    changed = true;
                }

                ImGui::PopID();
            }

            if (ImGui::Button("Add Tag"))
            {
                config.tags.push_back("NewTag");
                changed = true;
            }
        }
        ImGui::PopID();

        ImGui::PopItemWidth();

        if (changed)
            sys.SetConfig(config);
    }

    template<>
    void DisplayConfig<LayerManager>()
    {
        auto& sys = Core::GetSystem<LayerManager>();
        auto config = sys.GetConfig();

        const float item_width = ImGui::GetWindowContentRegionWidth() * 0.6f;
        const float pad_y = ImGui::GetStyle().FramePadding.y;
        bool changed = false;

        ImGui::PushItemWidth(item_width);

        ImGui::PushID("layers");
        if (ImGui::TreeNodeEx("Layers", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAllAvailWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen))
        {
            for (int i = 0; i < config.layers.size(); ++i)
            {
                if (i < LayerManager::num_builtin_layers)
                    ImGuidk::PushDisabled();

                const float cursor_y = ImGui::GetCursorPosY();
                ImGui::SetCursorPosY(cursor_y + pad_y);
                ImGui::Text(i < LayerManager::num_builtin_layers ? "Builtin Layer %d" : "User Layer %d", i);

                ImGui::SetCursorPosY(cursor_y);
                ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);

                ImGui::PushID(i);

                char buf[32];
                strcpy_s(buf, config.layers[i].data());

                ImGui::InputText("", buf, 32);
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    config.layers[i] = buf;
                    changed = true;
                }

                ImGui::PopID();

                if (i < LayerManager::num_builtin_layers)
                    ImGuidk::PopDisabled();
            }
        }
        ImGui::PopID();

        ImGui::PopItemWidth();

        if (changed)
            sys.SetConfig(config);
    }



    static const char* config_labels[]
    {
        "Tags and Layers"
    };
    void IGE_ProjectSettings::Update()
    {
        ImGui::Columns(2);

        ImGui::BeginChild("left");

        for (int i = 0; i < _max; ++i)
        {
            if (ImGui::Selectable(config_labels[i], _selection == i))
                _selection = i;
        }

        ImGui::EndChild();

        ImGui::NextColumn();

        ImGui::BeginChild("right");

        switch (_selection)
        {
        case _tags_and_layers:
            DisplayConfig<TagManager>();
            DisplayConfig<LayerManager>();
            break;
        default:
            break;
        }

        ImGui::EndChild();
    }

}