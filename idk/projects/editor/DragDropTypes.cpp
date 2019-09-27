#include "pch.h"

#include <imgui/imgui.h>
#include <res/GenericResourceHandle.h>

namespace idk
{

    void DragDrop::SetResourcePayload(const GenericResourceHandle& handle)
    {
        ImGui::SetDragDropPayload(RESOURCE, &handle, sizeof(handle));
    }

    const GenericResourceHandle& DragDrop::GetResourcePayloadData(const ImGuiPayload* payload)
    {
        return *reinterpret_cast<GenericResourceHandle*>(payload->Data);
    }

}