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

    template<>
    void DisplayConfig<PhysicsSystem>()
    {
        const auto& layers = Core::GetSystem<LayerManager>().GetConfig().layers;

        auto& sys = Core::GetSystem<PhysicsSystem>();
        auto config = sys.GetConfig();

        bool changed = false;

        ImVec2 max_label_size{};
        for(const auto& layer : layers)
        {
            if (layer.empty())
                continue;
            
            ImVec2 sz = ImGui::CalcTextSize(layer.c_str());
            if (sz.x > max_label_size.x) max_label_size.x = sz.x;
            if (sz.y > max_label_size.y) max_label_size.y = sz.y;
        }

        ImGui::Text("Collision Matrix");
        ImGui::Indent();

        const float base_x = ImGui::GetCursorPosX();
        const float base_y = ImGui::GetCursorPosY();
        const float spacing_x = ImGui::GetStyle().ItemInnerSpacing.x;
        size_t counter = 0;
        for (size_t i = 0; i < LayerManager::num_layers; ++i)
        {
            const size_t layer_index = LayerManager::num_layers - i - 1;
            const auto& layer = layers[layer_index];

            if (layer.empty())
                continue;

            ImGui::SetCursorPosX(base_x + max_label_size.x + spacing_x + ImGui::GetStyle().FramePadding.y +
                (ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetTextLineHeight() + spacing_x) * counter++);
            ImGui::SetCursorPosY(base_y + max_label_size.x - ImGui::CalcTextSize(layer.c_str()).x);
            ImGuidk::VerticalText(layer.c_str());
        }
        for (size_t i = 0; i < LayerManager::num_layers; ++i)
        {
            const auto& layer = layers[i];
            if (layer.empty())
                continue;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + max_label_size.x - ImGui::CalcTextSize(layer.c_str()).x);
            ImGui::Text(layer.c_str());

            for (size_t j = i; j < LayerManager::num_layers; ++j)
            {
                size_t layer_index = LayerManager::num_layers - j + i - 1;
                if (layers[layer_index].empty())
                    continue;

                ImGui::SameLine(0, spacing_x);
				bool checked = config.matrix[i] & LayerMask{ 1 << layer_index };
                if (ImGui::Checkbox(("##matrix" + std::to_string(i * LayerManager::num_layers + j)).c_str(), &checked))
                {
					config.matrix[i] = config.matrix[i] & LayerMask{ ~(1 << layer_index) | (checked << layer_index) };
                    config.matrix[layer_index] = config.matrix[layer_index] & LayerMask{~(1 << i) | (checked << i)};
                    changed = true;
                }
            }
        }

        ImGui::Unindent();

        if (changed)
        {
            sys.SetConfig(config);
        }
    }


    static const char* config_labels[]
    {
        "Tags and Layers",
        "Scenes",
        "Scripts",
        "Physics"
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
        case _scene:
            DisplayConfig<SceneManager>();
            break;
        case _script:
            DisplayConfig<mono::ScriptSystem>();
            break;
        case _physics:
            DisplayConfig<PhysicsSystem>();
            break;
        default:
            break;
        }

        ImGui::EndChild();
    }

}