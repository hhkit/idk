#include "pch.h"
#include "DragVec.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace idk::ImGuidk
{
    bool DragVec2(const char* label, vec2* vec, float speed, float min, float max)
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

        PushID(0);
        SetCursorPosY(y + g.Style.FramePadding.y);
        ImGui::Text("X");
        SameLine(axis_char_width + g.Style.ItemInnerSpacing.x);
        SetCursorPosY(y);
        changed |= DragFloat("", &vec->x, speed, min, max);
        SameLine();
        PopID();

        PushID(1);
        ImGui::Text("Y");
        SameLine(axis_char_width * 2 + g.Style.ItemInnerSpacing.x * 3 + drag_float_width);
        SetCursorPosY(y);
        changed |= DragFloat("", &vec->y, speed, min, max);
        SameLine();
        PopID();

        PopItemWidth();
        PopID();
        EndGroup();

        return changed;
    }

    bool DragVec3(const char* label, vec3* vec, float speed, float min, float max)
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
        const float drag_float_width = (full_width - axis_char_width * 3 - g.Style.ItemInnerSpacing.x * 5) / 3;

        PushItemWidth(drag_float_width);

        auto y = GetCursorPosY();

        PushID(0);
        SetCursorPosY(y + g.Style.FramePadding.y);
        ImGui::Text("X");
        SameLine(axis_char_width + g.Style.ItemInnerSpacing.x);
        SetCursorPosY(y);
        changed |= DragFloat("", &vec->x, speed, min, max);
        SameLine();
        PopID();

        PushID(1);
        ImGui::Text("Y");
        SameLine(axis_char_width * 2 + g.Style.ItemInnerSpacing.x * 3 + drag_float_width);
        SetCursorPosY(y);
        changed |= DragFloat("", &vec->y, speed, min, max);
        SameLine();
        PopID();

        PushID(2);
        ImGui::Text("Z");
        SameLine(axis_char_width * 3 + g.Style.ItemInnerSpacing.x * 5 + drag_float_width * 2);
        SetCursorPosY(y);
        changed |= DragFloat("", &vec->z, speed, min, max);
        SameLine();
        PopID();

        PopItemWidth();
        PopID();
        EndGroup();

        return changed;
    }

}