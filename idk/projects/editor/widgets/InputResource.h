#pragma once

#include <imgui/imgui.h>
#include <res/ResourceHandle.h>
#include <editor/ResourceExtensions.h>

namespace idk::ImGuidk
{
    template<typename ResT>
    bool InputResource(const char* label, idk::RscHandle<ResT>* handle)
    {
        //// todo: get path handle
        //PathHandle path = "/assets/models/test.fbx";

        //if (InputResourceEx(label, &path, RscExtensions<ResT>))
        //{
        //    *handle = Core::GetResourceManager().LoadFile(path)[0].As<ResT>();
        //}
        return false;
    }

    bool InputResourceEx(const char* label, idk::PathHandle* handle, idk::span<const char* const> accepted_extensions);
}