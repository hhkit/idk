#include "pch.h"
#include "InputResource.h"
#include <imgui/imgui_internal.h>

namespace idk
{
    bool ImGuidk::InputResourceEx(const char* label, idk::PathHandle* handle, idk::span<const char* const> accepted_extensions)
    {
        using namespace ImGui;

        auto* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        auto& style = GetStyle();

        const ImGuiID id = window->GetID(label);

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        ImVec2 size = CalcItemSize(ImVec2(0, 0), CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);
        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
        const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? (style.ItemInnerSpacing.x + label_size.x) : 0.0f, 0.0f));

        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, id, &frame_bb))
            return false;
        bool hovered = ItemHoverable(frame_bb, id);

        bool dropped = false;
        if (BeginDragDropTarget())
        {
            if (const auto* payload = AcceptDragDropPayload(DragDrop::RESOURCE, ImGuiDragDropFlags_AcceptPeekOnly))
            {
                auto path = *reinterpret_cast<idk::PathHandle*>(payload->Data);
                auto path_ext = path.GetExtension();

                if (path)
                {
                    for (auto ext : accepted_extensions)
                    {
                        if (ext != path_ext)
                            continue;

                        hovered = true;
                        if (payload->IsDelivery())
                        {
                            *handle = path;
                            dropped = true;
                        }
                        break;
                    }
                }
            }
            EndDragDropTarget();
        }

        const auto col = hovered ? ImGui::GetColorU32(ImGuiCol_FrameBgHovered) : ImGui::GetColorU32(ImGuiCol_FrameBg);

        ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, col, false);

        if (*handle)
        {
            const char* text = handle->GetMountPath().data() + sizeof("/assets/") - 1;
            ImGui::RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, text, 0, nullptr);
        }

        return dropped;
    }
}