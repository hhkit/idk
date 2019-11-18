/******************************************************************************/
/*!
\project	SientEditor
\file		imgui_verticaltext.cpp
\author		Hoe Kau Wee, Malody
\brief
Implementation for drawing vertical text in imgui
All content (C) 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"
#include "VerticalText.h"
#include <imgui/imgui_internal.h>

namespace idk::ImGuidk
{

    void VerticalText(const char* text)
    {
        using namespace ImGui;

        ImFont* font = GImGui->Font;
        ImVec2 text_size = CalcTextSize(text);
        auto* window = GetCurrentWindow();
        ImVec2 pos = window->DC.CursorPos;
        pos.x += text_size.y;

        const ImU32 text_color = ImGui::ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Text]);
        const char* pc = text;
        while (*pc)
        {
            auto* glyph = font->FindGlyph(*pc);
            if (!glyph) continue;

            window->DrawList->PrimReserve(6, 4);
            window->DrawList->PrimQuadUV(
                ImVec2(pos.x - glyph->Y0, pos.y + glyph->X0),
                ImVec2(pos.x - glyph->Y0, pos.y + glyph->X1),
                ImVec2(pos.x - glyph->Y1, pos.y + glyph->X1),
                ImVec2(pos.x - glyph->Y1, pos.y + glyph->X0),
                ImVec2(glyph->U0, glyph->V0),
                ImVec2(glyph->U1, glyph->V0),
                ImVec2(glyph->U1, glyph->V1),
                ImVec2(glyph->U0, glyph->V1),
                text_color);
            pos.y += glyph->AdvanceX;
            ++pc;
        }

        ItemSize(ImVec2{ GetTextLineHeight(), text_size.x });
    }

}