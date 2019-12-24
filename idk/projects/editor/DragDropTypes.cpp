#include "pch.h"
#include <imgui/imgui.h>

#include "DragDropTypes.h"
#include <res/GenericResourceHandle.h>
#include <ds/span.inl>

namespace idk
{
    static vector<GenericResourceHandle> resource_payload;

    void DragDrop::SetResourcePayload(const GenericResourceHandle& handle)
    {
        resource_payload.clear();
        resource_payload.push_back(handle);
        ImGui::SetDragDropPayload(RESOURCE, resource_payload.data(), sizeof(GenericResourceHandle*));
    }

    void DragDrop::SetResourcePayload(const ResourceBundle& bundle)
    {
        resource_payload = vector<GenericResourceHandle>(bundle.GetAll().begin(), bundle.GetAll().end());
        ImGui::SetDragDropPayload(RESOURCE, resource_payload.data(), sizeof(GenericResourceHandle*));
    }

    const vector<GenericResourceHandle>& DragDrop::GetResourcePayloadData()
    {
        return resource_payload;
    }

}