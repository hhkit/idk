#pragma once

struct ImGuiPayload;

namespace idk
{
    struct GenericResourceHandle;

    struct DragDrop
    {
        constexpr static auto PARAMETER = "dd_param";
        constexpr static auto RESOURCE  = "dd_resource";

        static void SetResourcePayload(const GenericResourceHandle& handle);
        static const GenericResourceHandle& GetResourcePayloadData(const ImGuiPayload* payload);
    };
}