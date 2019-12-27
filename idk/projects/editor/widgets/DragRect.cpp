#include "pch.h"
#include "DragVec.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <math/rect.h>

namespace idk::ImGuidk
{
    bool DragRect(const char* label, rect* rect, float speed, float min, float max)
    {
        using namespace ImGui;

        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        bool changed = false;
        BeginGroup();
        PushID(label);

        const float full_width = CalcItemWidth();
        const float axis_char_width = CalcTextSize("Z").x * 1.5f;
        const float drag_float_width = (full_width - axis_char_width * 2 - g.Style.ItemInnerSpacing.x * 3) / 2;

        PushItemWidth(drag_float_width);

        auto y = GetCursorPosY();
        auto x = GetCursorPosX();

        PushID(0);
        SetCursorPosY(y + g.Style.FramePadding.y);
        ImGui::Text("X");
        SameLine(axis_char_width + g.Style.ItemInnerSpacing.x);
        SetCursorPosY(y);
        changed |= DragFloat("", &rect->position.x, speed, min, max);
        SameLine();
        PopID();

        PushID(1);
        ImGui::Text("Y");
        SameLine(axis_char_width * 2 + g.Style.ItemInnerSpacing.x * 3 + drag_float_width);
        SetCursorPosY(y);
        changed |= DragFloat("", &rect->position.y, speed, min, max);
        PopID();

        y = GetCursorPosY();
        SetCursorPosX(x);
        PushID(2);
        SetCursorPosY(y + g.Style.FramePadding.y);
        ImGui::Text("W");
        SameLine(axis_char_width + g.Style.ItemInnerSpacing.x);
        SetCursorPosY(y);
        changed |= DragFloat("", &rect->size.x, speed, min, max);
        SameLine();
        PopID();

        PushID(3);
        ImGui::Text("H");
        SameLine(axis_char_width * 2 + g.Style.ItemInnerSpacing.x * 3 + drag_float_width);
        SetCursorPosY(y);
        changed |= DragFloat("", &rect->size.y, speed, min, max);
        PopID();

        PopItemWidth();
        PopID();
        EndGroup();

        return changed;
    }
}