#include "pch.h"
#include "DragQuat.h"
#include <editor/widgets/DragVec3.h>
#include <math/euler_angles.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace idk::ImGuidk
{

    bool DragQuat(const char* label, quat* q, float speed, float min, float max)
    {
        using namespace ImGui;

        static euler_angles active_euler{ *q };
        euler_angles euler{ *q };
        static ImGuiID active_id;

        ImGuiContext& g = *GImGui;
        auto id = GetID(label);
        if (active_id != id)
            euler = euler_angles{ *q };
        else
            euler = active_euler;

        vec3 deg_euler{ deg(euler.x).value, deg(euler.y).value, deg(euler.z).value };

        bool ret = false;

        if (DragVec3(label, &deg_euler, speed, min, max))
        {
            active_euler.x = rad(deg(deg_euler.x));
            active_euler.y = rad(deg(deg_euler.y));
            active_euler.z = rad(deg(deg_euler.z));
            *q = quat(active_euler);
            active_id = id;
            ret = true;
        }

        if (IsItemDeactivatedAfterEdit())
            active_id = 0;

        return ret;
    }

}
