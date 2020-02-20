#pragma once

#include <meta/meta.h>
#include <idk.h>

namespace idk::ImGuidk
{
    template<typename EnumT, typename = sfinae<is_macro_enum_v<EnumT>>>
    bool EnumCombo(const char* label, EnumT* e)
    {
        bool ret = false;

        if (ImGui::BeginCombo(label, string(e->to_string()).c_str()))
        {
            for (auto name : EnumT::names)
            {
                if (ImGui::MenuItem(string(name).c_str()))
                {
                    *e = EnumT::from_string(name);
                    ret = true;
                }
            }
            ImGui::EndCombo();
        }

        return ret;
    }
}