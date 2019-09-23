#pragma once

#include <imgui/imgui.h>
#include <res/ResourceHandle.h>
#include <file/PathHandle.h>

namespace ImGuidk
{
    template<typename ResT>
    bool InputResource(const char* label, idk::RscHandle<ResT>* handle)
    {
        
    }

    bool InputResourceEx(const char* label, idk::PathHandle* handle, const std::vector<const char*>& accepted_extensions);
}