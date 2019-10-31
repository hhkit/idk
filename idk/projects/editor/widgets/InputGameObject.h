#pragma once

#include <imgui/imgui.h>
#include <core/GameObject.h>

namespace idk::ImGuidk
{
    bool InputGameObject(const char* label, Handle<GameObject>* handle);
}