#include "pch.h"
#include "imguidk.h"
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>

namespace idk::ImGuidk
{
    void PushDisabled()
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetColorU32(ImGuiCol_TitleBg));
    }
    void PopDisabled()
    {
        ImGui::PopItemFlag();
        ImGui::PopStyleColor(2);
    }
}