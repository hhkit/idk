#include "pch.h"
#include "IGE_InspectorWindow.h"

#include <editor/imguidk.h>
#include <editor/utils.h>
#include <gfx/ShaderGraph.h>
#include <prefab/PrefabUtility.h>
#include <IncludeComponents.h>
#include <IncludeResources.h>
#include <imgui/imgui_internal.h>

namespace idk
{
    static string get_asset_name(const GenericResourceHandle& handle)
    {
        return std::visit([](auto h) -> string
        {
            auto name = h->Name();
            auto path = Core::GetResourceManager().GetPath(h);
            if (!name.empty())
                return string{ name };
            else if (path)
                return string{ PathHandle{ *path }.GetStem() };
            return "";
        }, handle);
    }

    void IGE_InspectorWindow::DisplayAsset(GenericResourceHandle handle)
    {
        ImGui::Text(get_asset_name(handle).c_str());

        std::visit([&](auto h)
        {
            using ResT = typename decltype(h)::Resource;
            auto path = Core::GetResourceManager().GetPath(h);

            ImGui::Text(format_name(reflect::get_type<ResT>().name()).c_str());
            ImGui::Text(string(handle.guid()).c_str());

            if constexpr (std::is_same_v<ResT, Prefab>)
                DisplayAsset(h);

            if constexpr (std::is_same_v<ResT, MaterialInstance>)
                DisplayAsset(h);

            if constexpr (std::is_same_v<ResT, Material>)
                DisplayAsset(h);

        }, handle);
    }

    void IGE_InspectorWindow::DisplayAsset(RscHandle<Prefab> prefab)
    {
        auto& data = prefab->data[0];

        auto name = reflect::get_type<Transform>().name();
        auto trans = data.FindComponent(name);
        if (trans.valid())
        {
            ImGui::PushID(0);
            if (ImGui::CollapsingHeader(name.data(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto& c = trans.get<Transform>();

                const float item_width = ImGui::GetWindowContentRegionWidth() * 0.75f;
                const float pad_y = ImGui::GetStyle().FramePadding.y;

                ImGui::PushItemWidth(item_width);

                auto y = ImGui::GetCursorPosY();
                ImGui::SetCursorPosY(y + pad_y);
                ImGui::Text("Position");
                ImGui::SetCursorPosY(y);
                ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);
                ImGuidk::DragVec3("##0", &c.position);

                y = ImGui::GetCursorPosY();
                ImGui::SetCursorPosY(y + pad_y);
                ImGui::Text("Rotation");
                ImGui::SetCursorPosY(y);
                ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);
                ImGuidk::DragQuat("##1", &c.rotation);

                y = ImGui::GetCursorPosY();
                ImGui::SetCursorPosY(y + pad_y);
                ImGui::Text("Scale");
                ImGui::SetCursorPosY(y);
                ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);
                if (ImGuidk::DragVec3("##2", &c.scale))
                {
                    PrefabUtility::PropagatePropertyToInstances(prefab, 0, name, "scale");
                    prefab->Dirty();
                }

                ImGui::PopItemWidth();
            }
            ImGui::PopID();
        }

        auto& components = prefab->data[0].components;

        int i = 0;
        for (auto& c : components)
        {
            ++i;
            if (c.type == reflect::get_type<Transform>() ||
                c.type == reflect::get_type<Name>())
                continue;

            auto cursor_pos = ImGui::GetCursorPos();

            ImGui::PushID(i);
            if (ImGui::CollapsingHeader(c.type.name().data(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
            {
                if (displayVal(c))
                {
                    PrefabUtility::PropagatePrefabChangesToInstances(prefab);
                    prefab->Dirty();
                }
            }

            auto cursor_pos2 = ImGui::GetCursorPos();

            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - 10);
            ImGui::SetCursorPosY(cursor_pos.y);
            if (ImGui::Button("..."))
            {
                ImGui::OpenPopup("AdditionalOptions");
            }

            if (ImGui::BeginPopup("AdditionalOptions", ImGuiWindowFlags_None))
            {
                if (ImGui::MenuItem("Reset"))
                {

                }
                ImGui::Separator();
                if (ImGui::MenuItem("Remove Component"))
                {
                    PrefabUtility::RemoveComponentFromPrefab(prefab, 0, i - 1);
                    prefab->Dirty();
                }
                ImGui::EndPopup();
            }

            ImGui::SetCursorPos(cursor_pos2);

            ImGui::PopID();
        }

        ImGui::Separator();
        ImGui::SetCursorPosX(window_size.x * 0.25f);
        if (ImGui::Button("Add Component", ImVec2{ window_size.x * 0.5f, 0.0f }))
            ImGui::OpenPopup("AddComp");

        if (ImGui::BeginPopup("AddComp"))
        {
            for (const char* c_name : GameState::GetComponentNames())
            {
                string displayName = c_name;
                if (displayName == "Transform" ||
                    displayName == "Name" ||
                    displayName == "Tag" ||
                    displayName == "Layer" ||
                    displayName == "PrefabInstance")
                    continue;

                if (ImGui::MenuItem(displayName.c_str()))
                {
                    PrefabUtility::AddComponentToPrefab(prefab, 0, reflect::get_type(c_name).create());
                }
            }
            ImGui::EndPopup();
        }
    }

    void IGE_InspectorWindow::DisplayAsset(RscHandle<MaterialInstance> material)
    {
        const float item_width = ImGui::GetWindowContentRegionWidth() * item_width_ratio;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        auto graph = RscHandle<shadergraph::Graph>{ material->material };
        bool changed = false;

        for (auto& [name, u] : material->material->uniforms)
        {
            const auto y = ImGui::GetCursorPosY();

            ImGui::SetCursorPosY(y + pad_y);

            ImGui::PushID(name.c_str());

            bool has_override = material->uniforms.find(name) != material->uniforms.end();
            if (ImGui::Checkbox("##override", &has_override))
            {
                if (has_override)
                    material->uniforms[name] = material->material->uniforms[name].value;
                else
                    material->uniforms.erase(name);
            }
            ImGui::SameLine();

            ImGui::PushItemWidth(item_width);

            if (!has_override)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetColorU32(ImGuiCol_TitleBg));
            }

            ImGui::Text(name.c_str());
            ImGui::SetCursorPosY(y);
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);

            switch (u.value.index())
            {
            case index_in_variant_v<float, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<float, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGui::DragFloat("", &val, 0.01f))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<vec2, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<vec2, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGuidk::DragVec2("", &val, 0.01f))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<vec3, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<vec3, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGuidk::DragVec3("", &val, 0.01f))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<vec4, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<vec4, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGui::DragFloat4("", val.data(), 0.01f))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<RscHandle<Texture>, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<RscHandle<Texture>, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGuidk::InputResource("", &val))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }

            default:
                break;
            }

            if (!has_override)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleColor(2);
            }

            ImGui::PopItemWidth();
            ImGui::PopID();
        }

        if (changed)
            material->Dirty();
    }

    void IGE_InspectorWindow::DisplayAsset(RscHandle<Material> material)
    {
        const float item_width = ImGui::GetWindowContentRegionWidth() * item_width_ratio;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        auto graph = RscHandle<shadergraph::Graph>{ material };

        //for (auto& [name, u] : material->uniforms)
        //{
        //    if (u.index() == index_in_variant_v<RscHandle<Texture>, UniformInstance>)
        //        continue;

        //    auto y = ImGui::GetCursorPosY();

        //    ImGui::SetCursorPosY(y + pad_y);

        //    ImGui::PushID(name.c_str());
        //    ImGui::PushItemWidth(item_width);

        //    auto label = graph->parameters[std::stoi(name.data() + sizeof("_ub0._u") - 1)].name.c_str();
        //    switch (u.index())
        //    {
        //    case index_in_variant_v<float, UniformInstance>:
        //        ImGui::Text(label);
        //        ImGui::SetCursorPosY(y);
        //        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);
        //        ImGui::DragFloat("", &std::get<index_in_variant_v<float, UniformInstance>>(u), 0.01f);

        //    default:
        //        break;
        //    }

        //    ImGui::PopItemWidth();
        //    ImGui::PopID();
        //}
    }

}