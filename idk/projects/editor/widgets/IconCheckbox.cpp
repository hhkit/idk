#include "pch.h"
#include "IconCheckbox.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <editor/imguidk.h>

namespace idk::ImGuidk
{

    bool IconCheckbox(const char* label, const char* icon, bool* v)
    {
        using namespace ImGui;

        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImVec2 icon_size = CalcTextSize(icon, NULL, true);

        const float square_sz = GetFrameHeight();
        const ImVec2 pos = window->DC.CursorPos;
        const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, id))
            return false;

        bool hovered, held;
        bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
        if (pressed)
        {
            *v = !(*v);
            MarkItemEdited(id);
        }

        const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
        RenderNavHighlight(total_bb, id);

        ImGui::PushFont(g.IO.Fonts->Fonts[0]);
        if (*v) PushDisabled();
        RenderText(check_bb.Min + (check_bb.GetSize() - icon_size) * 0.5f, icon);
        if (*v) PopDisabled();
        ImGui::PopFont();

        if (label_size.x > 0.0f)
            RenderText(ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y), label);

        return pressed;
    }

}
