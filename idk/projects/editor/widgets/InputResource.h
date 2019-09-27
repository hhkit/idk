#pragma once

#include <imgui/imgui.h>
#include <res/GenericResourceHandle.h>
#include <editor/ResourceExtensions.h>

namespace idk::ImGuidk
{
    bool InputResourceEx(const char* label, GenericResourceHandle* handle, size_t base_resource_id);

    template<typename ResT>
    bool InputResource(const char* label, idk::RscHandle<ResT>* handle)
    {
        GenericResourceHandle h{ *handle };
        if (InputResourceEx(label, &h, BaseResourceID<ResT>))
        {
            *handle = h.AsHandle<ResT>();
            return true;
        }
        return false;
    }
}