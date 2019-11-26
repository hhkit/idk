#include "pch.h"
#include "LayerMaskCombo.h"
#include <common/LayerManager.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace idk::ImGuidk
{
    bool LayerMaskCombo(const char* label, LayerMask* layer_mask)
    {
        const auto& layer_manager = Core::GetSystem<LayerManager>();
        const auto& layers = layer_manager.GetConfig().layers;
        size_t num_layers_to_show = 0;
        LayerMask all_shown_layers;

        for (int i = 0; i < layers.size(); ++i)
        {
            if (layers[i].empty())
                continue;
            ++num_layers_to_show;
            all_shown_layers.mask |= 1 << i;
        }

        string_view display_str = "Nothing";

        auto mask = layer_mask->mask;
        if (mask && !(mask & (mask - 1))) // is PoT, so just 1 layer selected
        {
            LayerManager::layer_t layer = 0;
            auto copy = mask;
            while (copy >>= 1)
                ++layer;
            display_str = layer_manager.LayerIndexToName(layer);
        }

        else if ((mask & all_shown_layers.mask) == all_shown_layers.mask)
            display_str = "Everything";
        else if (mask)
            display_str = "Mixed";

        const float ticksize = ImGui::GetFontSize() * 0.75f;
        const auto text_color = ImGui::GetColorU32(ImGuiCol_Text);
        if (ImGui::BeginCombo(label, display_str.data()))
        {
            if (mask == 0)
                ImGui::RenderCheckMark(ImGui::GetCursorScreenPos() + vec2{ 0, ImGui::GetStyle().ItemInnerSpacing.y }, text_color, ticksize);
            if (ImGui::Selectable("\t Nothing"))
                layer_mask->mask = 0;

            if ((mask & all_shown_layers) == all_shown_layers)
                ImGui::RenderCheckMark(ImGui::GetCursorScreenPos() + vec2{ 0, ImGui::GetStyle().ItemInnerSpacing.y }, text_color, ticksize);
            if (ImGui::Selectable("\t Everything"))
                layer_mask->mask = all_shown_layers.mask;

            for (int i = 0; i < layers.size(); ++i)
            {
                if (layers[i].empty())
                    continue;
                if (layer_mask->mask & (1 << i))
                    ImGui::RenderCheckMark(ImGui::GetCursorScreenPos() + vec2{ 0, ImGui::GetStyle().ItemInnerSpacing.y }, text_color, ticksize);
                if (ImGui::Selectable(("\t " + layers[i]).c_str()))
                    layer_mask->mask = layer_mask->mask ^ (1 << i);
            }
            ImGui::EndCombo();
        }

        return mask != layer_mask->mask;
    }
}