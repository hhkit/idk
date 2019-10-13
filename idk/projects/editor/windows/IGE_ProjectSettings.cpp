#include "pch.h"
#include "IGE_ProjectSettings.h"
#include <IncludeSystems.h>
#include <editor/utils.h>
#include <editor/imguidk.h>

namespace idk
{

    IGE_ProjectSettings::IGE_ProjectSettings()
        : IGE_IWindow{ "Project Settings", false }
    {
    }

    void IGE_ProjectSettings::BeginWindow()
    {
    }



    static bool displayVal(reflect::dynamic dyn)
    {
        const float item_width = ImGui::GetWindowContentRegionWidth() * 0.6f;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        bool outer_changed = false;

        dyn.visit([&](auto&& key, auto&& val, int /*depth_change*/)
        {
            using K = std::decay_t<decltype(key)>;
            using T = std::decay_t<decltype(val)>;

            const float currentHeight = ImGui::GetCursorPosY();

            if constexpr (std::is_same_v<K, reflect::type>) // from variant visit
                return true;
            else if constexpr (!std::is_same_v<K, const char*>)
                throw "Unhandled case";
            else
            {
                string keyName = format_name(key);

                ImGui::BeginGroup();

                ImGui::SetCursorPosY(currentHeight + pad_y);
                ImGui::Text("");

                ImGui::SetCursorPosY(currentHeight);
                ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);

                ImGui::PushID("");
                ImGui::PushItemWidth(item_width);

                bool changed = false;
                bool recurse = false;

                //ALL THE TYPE STATEMENTS HERE
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, real>)
                {
                    changed |= ImGui::DragFloat("", &val);
                }
                else if constexpr (std::is_same_v<T, int>)
                {
                    changed |= ImGui::DragInt("", &val);
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    changed |= ImGui::Checkbox("", &val);
                }
                else if constexpr (std::is_same_v<T, vec3>)
                {
                    changed |= ImGuidk::DragVec3("", &val);
                }
                else if constexpr (std::is_same_v<T, quat>)
                {
                    changed |= ImGuidk::DragQuat("", &val);
                }
                else if constexpr (std::is_same_v<T, color>)
                {
                    changed |= ImGui::ColorEdit4("", val.data());
                }
                else if constexpr (std::is_same_v<T, rad>)
                {
                    changed |= ImGui::SliderAngle("", val.data());
                }
                else if constexpr (is_template_v<T, RscHandle>)
                {
                    if (ImGuidk::InputResource("", &val))
                    {
                        changed = true;
                    }
                }
                else if constexpr (is_template_v<T, std::variant>)
                {
                    const int curr_ind = s_cast<int>(val.index());
                    int new_ind = curr_ind;

                    constexpr auto sz = reflect::detail::pack_size<T>::value; // THE FUUU?
                    using VarCombo = std::array<const char*, sz>;

                    static auto combo_items = []()-> VarCombo
                    {
                        constexpr auto sz = reflect::detail::pack_size<T>::value; // THE FUUU?
                        static std::array<string, sz> tmp_arr;
                        std::array<const char*, sz> retval{};

                        auto sp = reflect::unpack_types<T>();

                        for (size_t i = 0; i < sz; ++i)
                        {
                            tmp_arr[i] = format_name(sp[i].name());
                            retval[i] = tmp_arr[i].data();
                        }
                        return retval;
                    }();

                    if (ImGui::Combo(keyName.data(), &new_ind, combo_items.data(), static_cast<int>(sz)))
                    {
                        val = variant_construct<T>(new_ind);
                        changed = true;
                    }

                    recurse = true;
                }
                else if constexpr (is_sequential_container_v<T>)
                {
                    reflect::uni_container cont{ val };
                    ImGui::Button("+");
                    ImGui::Indent();
                    for (auto dyn : cont)
                    {
                        displayVal(dyn);
                    }
                    ImGui::Unindent();
                }
                else if constexpr (is_associative_container_v<T>)
                {
                    ImGui::Text("Associative Container");
                    /*reflect::uni_container cont{ val };
                    ImGui::Button("+");
                    ImGui::Indent();
                    for (auto dyn : cont)
                    {
                        auto pair = dyn.unpack();
                        displayVal(pair[0]);
                    }
                    ImGui::Unindent();*/
                }
                else
                {
                    ImGui::NewLine();
                    ImGui::Indent();
                    displayVal(val);
                    ImGui::Unindent();
                    /*ImGui::SetCursorPosY(currentHeight + heightOffset);
                    ImGui::TextDisabled("Member type not defined in IGE_InspectorWindow::Update");*/
                }

                ImGui::EndGroup();

                ImGui::PopItemWidth();
                ImGui::PopID();

                outer_changed |= changed;

                return recurse;
            }

        });

        return outer_changed;
    }


    template<typename T>
    static void DisplayConfig()
    {
    }

    template<>
    void DisplayConfig<TagSystem>()
    {
        auto& sys = Core::GetSystem<TagSystem>();
        const auto& config = sys.GetConfig();

        displayVal(config);
    }


    enum config
    {
        _tags_and_layers = 0,
        _max
    };
    static const char* config_labels[]
    {
        "Tags and Layers"
    };
    void IGE_ProjectSettings::Update()
    {
        ImGui::Columns(2);

        ImGui::BeginChild("left");

        for (int i = 0; i < _max; ++i)
        {
            if (ImGui::Selectable(config_labels[i], _selection == i))
                _selection = i;
        }

        ImGui::EndChild();

        ImGui::NextColumn();

        ImGui::BeginChild("right");

        switch (_selection)
        {
        case _tags_and_layers:
            DisplayConfig<TagSystem>();
            break;
        default:
            break;
        }

        ImGui::EndChild();
    }

}