#include "pch.h"
#include "InputResource.h"
#include <imgui/imgui_internal.h>
#include <editor/DragDropTypes.h>
#include <scene/Scene.h>
#include <res/ResourceHandle.inl>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceUtils.inl>
#include <IncludeResources.h>
#include <map>

namespace idk
{
    bool ImGuidk::InputResourceEx(const char* label, GenericResourceHandle* handle, [[maybe_unused]] size_t base_resource_id)
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
                auto res_payload = DragDrop::GetResourcePayloadData();

                for (auto& h : res_payload)
                {
                    if (h.resource_id() == handle->resource_id())
                    {
                        hovered = true;
                        if (payload->IsDelivery())
                        {
                            *handle = h;
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

        string text;
        if (handle)
        {
            text = std::visit([](auto h) -> string
            {
                auto name = h->Name();
                auto path = Core::GetResourceManager().GetPath(h);
                if (!name.empty())
                    return string{ name };
                else if (path)
                    return string{ PathHandle{ *path }.GetStem() };
                return "";
            }, *handle);
        }
        if (text.empty())
            text = "None";

        ImGui::RenderTextClipped(frame_bb.Min + style.FramePadding,
            frame_bb.Max - style.FramePadding,
            text.data(), 0, nullptr);

        if (label_size.x > 0)
            RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);
		
        if (IsItemClicked())
        {
            OpenPopup(label);
            SetNextWindowPos(frame_bb.GetBL());
        }
        if (BeginPopup(label))
        {
            std::visit([&](auto h)
            {
                using T = typename decltype(h)::Resource;
                auto all_handles = Core::GetResourceManager().GetAll<T>();
                std::map<string, RscHandle<T>> table;

                for (auto handle_i : all_handles)
                {
                    auto name = handle_i->Name();
                    auto path = Core::GetResourceManager().GetPath(handle_i);
                    auto str = name.empty() ? string{ path->substr(0, path->rfind('.')) } : string{ name };
                    table.emplace(str, handle_i);
                }

                const T& default_res = *RscHandle<T>();

                if (MenuItem(default_res.Name().size() ? default_res.Name().data() : "None"))
                {
                    *handle = RscHandle<T>{};
                    dropped = true;
                }

                for (auto& [name, handle_i] : table)
                {
                    if (MenuItem(name.c_str()))
                    {
                        *handle = handle_i;
                        dropped = true;
                    }
                }
            }, *handle);

            ImGui::EndPopup();
        }

		std::visit([&](auto h)
		{
				using T = typename decltype(h)::Resource;
				if constexpr (std::is_same_v<T, idk::Texture>)
				{
					if (hovered)
					{
						ImGui::BeginTooltip();
						ImGui::Image(h->ID(), ImVec2{ 512, 512 }, ImVec2(0, 1), ImVec2(1, 0));
						ImGui::EndTooltip();
					}
				}
		}, *handle);

        return dropped;
    }
}