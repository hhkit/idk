

#include "pch.h"
#include "IGE_InspectorWindow.h"

#include <IDE.h>
#include <gfx/ShaderGraph.h>
#include <editor/commands/CommandList.h>
#include <editor/imguidk.h>
#include <editor/windows/IGE_HierarchyWindow.h>
#include <editor/windows/IGE_ProjectWindow.h>
#include <editor/windows/IGE_AnimatorWindow.h>
#include <editor/windows/IGE_ProjectSettings.h>
#include <editor/DragDropTypes.h>
#include <editor/utils.h>
#include <editor/ComponentIcons.h>
#include <editor/SceneManagement.h>

#include <common/TagManager.h>
#include <common/LayerManager.h>
#include <anim/AnimationSystem.h>
#include <anim/Animator.inl>
#include <app/Application.h>
#include <ds/span.h>
#include <reflect/reflect.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceUtils.inl>
#include <scene/SceneManager.h>
#include <math/euler_angles.h>
#include <meta/variant.h>
#include <script/MonoBehaviorEnvironment.h>
#include <prefab/PrefabUtility.h>
#include <core/Handle.inl>
#include <script/ScriptSystem.h>
#include <script/ManagedObj.inl>
#include <serialize/text.inl>
#include <util/property_path.h>

#include <IncludeComponents.h>
#include <IncludeResources.h>

#include <imgui/imgui_stl.h>
#include <imgui/imgui_internal.h> //InputTextEx
#include <iostream>

#include <gfx/GraphicsSystem.h>
#include <ds/span.inl>
#include <ds/result.inl>
namespace idk
{

    
    void IGE_InspectorWindow::DisplayComponentInner(Handle<Transform> c_transform)
    {
        ImVec2 cursorPos = ImGui::GetCursorPos();
        ImVec2 cursorPos2{};
        IDE& editor = Core::GetSystem<IDE>();

        bool has_changed = false;
        static vector<mat4> original_matrices;
        static auto check_modify = [&has_changed, &editor]()
        {
            if (ImGui::IsItemActive() && ImGui::GetCurrentContext()->ActiveIdIsJustActivated)
            {
                original_matrices.clear();
                for (auto i : editor.GetSelectedObjects().game_objects)
                    original_matrices.push_back(i->GetComponent<Transform>()->GlobalMatrix());
            }
            if (ImGui::IsItemDeactivatedAfterEdit())
                has_changed = true;
        };

        constexpr auto has_override = [](Handle<PrefabInstance> prefab_inst, const char* prop)
        {
            if (!prefab_inst)
                return false;
            for (const auto& ov : prefab_inst->overrides)
            {
                if (ov.component_name == reflect::get_type<Transform>().name() &&
                    ov.property_path == prop)
                {
                    return true;
                }
            }
            return false;
        };

        auto& c = *c_transform;

        const float item_width = ImGui::GetWindowContentRegionWidth() * 0.75f;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        ImGui::PushItemWidth(-4.0f);

        const bool has_pos_override = has_override(_prefab_inst, "position") && _prefab_inst->object_index > 0;
        auto y = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(y + pad_y);
        if (has_pos_override) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
        ImGui::Text("Position");
        if (has_pos_override) ImGui::PopStyleColor();
        ImGui::SetCursorPosY(y);
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);
        if (ImGuidk::DragVec3("##0", &c.position))
        {
            for (auto i : editor.GetSelectedObjects().game_objects)
            {
                if (i)
                    i->GetComponent<Transform>()->position = c.position;
            }
            if (_prefab_inst)
            {
                PrefabUtility::RecordPrefabInstanceChange(c_transform->GetGameObject(), c_transform, "position");
            }
        }
        check_modify();

        const bool has_rot_override = has_override(_prefab_inst, "rotation") && _prefab_inst->object_index > 0;
        y = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(y + pad_y);
        if (has_rot_override) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
        ImGui::Text("Rotation");
        if (has_rot_override) ImGui::PopStyleColor();
        ImGui::SetCursorPosY(y);
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);
        if (ImGuidk::DragQuat("##1", &c.rotation))
        {
            for (auto i : editor.GetSelectedObjects().game_objects)
            {
                if (i)
                    i->GetComponent<Transform>()->rotation = c.rotation;
            }
            if (_prefab_inst)
            {
                PrefabUtility::RecordPrefabInstanceChange(c_transform->GetGameObject(), c_transform, "rotation");
            }
        }
        check_modify();

        const bool has_scale_override = has_override(_prefab_inst, "scale");
        y = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(y + pad_y);
        if (has_scale_override) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
        ImGui::Text("Scale");
        if (has_scale_override) ImGui::PopStyleColor();
        ImGui::SetCursorPosY(y);
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);
        if (ImGuidk::DragVec3("##2", &c.scale))
        {
            for (auto i : editor.GetSelectedObjects().game_objects)
            {
                if (i)
                    i->GetComponent<Transform>()->scale = c.scale;
            }
            if (_prefab_inst)
            {
                PrefabUtility::RecordPrefabInstanceChange(c_transform->GetGameObject(), c_transform, "scale");
            }
        }
        check_modify();

        ImGui::PopItemWidth();


        if (has_changed)
        {
            int execute_counter = 0;
            for (size_t i = 0; i < editor.GetSelectedObjects().game_objects.size(); ++i)
            {
                const auto h = editor.GetSelectedObjects().game_objects[i];
                if (!h)
                    continue;
                const mat4 modified_mat = h->GetComponent<Transform>()->GlobalMatrix();
                editor.ExecuteCommand<CMD_TransformGameObject>(h, original_matrices[i], modified_mat);
                ++execute_counter;
            }
            Core::GetSystem<IDE>().ExecuteCommand<CMD_CollateCommands>(execute_counter);
        }
    }


    
    void IGE_InspectorWindow::DisplayComponentInner(Handle<RectTransform> c_rt)
    {
        if (c_rt->GetGameObject()->HasComponent<Canvas>())
        {
            ImGuidk::PushDisabled();
            ImGui::Text("Values driven by Canvas.");
            ImGuidk::PopDisabled();
            return;
        }

        const float region_width = ImGui::GetWindowContentRegionWidth();

        const auto w = region_width * 0.3f;
        ImGui::PushItemWidth(w);

        bool has_override = _prefab_inst &&
            (_prefab_inst->HasOverride((*_curr_component).type.name(), "offset_min", 0) ||
                _prefab_inst->HasOverride((*_curr_component).type.name(), "offset_max", 0));

        bool changed = false;

        ImGui::BeginGroup();

        if (c_rt->anchor_min.y != c_rt->anchor_max.y)
        {
            ImGui::SetCursorPosX(region_width * 0.5f - ImGui::CalcTextSize("T").x * 0.5f);
            ImGui::Text("T");
            ImGui::SetCursorPosX(region_width * 0.35f);
            changed |= ImGui::DragFloat("##top", &c_rt->offset_max.y);
        }
        else
        {
            ImGui::SetCursorPosX(region_width * 0.5f - ImGui::CalcTextSize("Y").x * 0.5f);
            ImGui::Text("Y");
            ImGui::SetCursorPosX(region_width * 0.35f);
            const float pivot_y = c_rt->_local_rect.position.y + c_rt->pivot.y * c_rt->_local_rect.size.y;
            float pos_y = pivot_y;
            if (ImGui::DragFloat("##pos_y", &pos_y))
            {
                float dy = pos_y - pivot_y;
                c_rt->offset_min.y += dy;
                c_rt->offset_max.y += dy;
                changed = true;
            }
        }

        if (c_rt->anchor_min.x != c_rt->anchor_max.x)
        {
            ImGui::SetCursorPosX(region_width * 0.35f - w * 0.75f - ImGui::CalcTextSize("L").x - ImGui::GetStyle().ItemSpacing.x);
            ImGui::Text("L");
            ImGui::SameLine();
            changed |= ImGui::DragFloat("##left", &c_rt->offset_min.x);
            ImGui::SameLine();
            ImGui::SetCursorPosX(region_width * 0.35f + w * 0.75f);
            changed |= ImGui::DragFloat("##right", &c_rt->offset_max.x);
            ImGui::SameLine();
            ImGui::Text("R");
        }
        else
        {
            ImGui::SetCursorPosX(region_width * 0.35f - w * 0.75f - ImGui::CalcTextSize("X").x - ImGui::GetStyle().ItemSpacing.x);
            ImGui::Text("X");
            ImGui::SameLine();

            const float pivot_x = c_rt->_local_rect.position.x + c_rt->pivot.x * c_rt->_local_rect.size.x;
            float pos_x = pivot_x;
            if (ImGui::DragFloat("##pos_x", &pos_x))
            {
                float dx = pos_x - pivot_x;
                c_rt->offset_min.x += dx;
                c_rt->offset_max.x += dx;
                changed = true;
            }
            ImGui::SameLine();

            float width = c_rt->_local_rect.size.x;
            ImGui::SetCursorPosX(region_width * 0.35f + w * 0.75f);
            if (ImGui::DragFloat("##width", &width))
            {
                c_rt->offset_min.x = pos_x - c_rt->pivot.x * width;
                c_rt->offset_max.x = pos_x + (1.0f - c_rt->pivot.x) * width;
                changed = true;
            }
            ImGui::SameLine();
            ImGui::Text("W");
        }

        if (c_rt->anchor_min.y != c_rt->anchor_max.y)
        {
            ImGui::SetCursorPosX(region_width * 0.35f);
            changed |= ImGui::DragFloat("##bot", &c_rt->offset_min.y);
            ImGui::SetCursorPosX(region_width * 0.5f - ImGui::CalcTextSize("B").x * 0.5f);
            ImGui::Text("B");
        }
        else
        {
            float height = c_rt->_local_rect.size.y;
            ImGui::SetCursorPosX(region_width * 0.35f);
            if (ImGui::DragFloat("##height", &height))
            {
                const float pivot_y = c_rt->_local_rect.position.y + c_rt->pivot.y * c_rt->_local_rect.size.y;
                c_rt->offset_min.y = pivot_y - c_rt->pivot.y * height;
                c_rt->offset_max.y = pivot_y + (1.0f - c_rt->pivot.y) * height;
                changed = true;
            }
            ImGui::SetCursorPosX(region_width * 0.5f - ImGui::CalcTextSize("B").x * 0.5f);
            ImGui::Text("H");
        }

        ImGui::PopItemWidth();

        ImGui::EndGroup();

        if (has_override && ImGui::BeginPopupContextItem("__context"))
        {
            if (ImGui::MenuItem("Apply Property"))
            {
                PropertyOverride ov{ string((*_curr_component).type.name()), "offset_min", 0 };
                PrefabUtility::ApplyPropertyOverride(_prefab_inst->GetGameObject(), ov);
                ov.property_path = "offset_max";
                PrefabUtility::ApplyPropertyOverride(_prefab_inst->GetGameObject(), ov);
            }
            if (ImGui::MenuItem("Revert Property"))
            {
                PropertyOverride ov{ string((*_curr_component).type.name()), "offset_min", 0 };
                PrefabUtility::RevertPropertyOverride(_prefab_inst->GetGameObject(), ov);
                ov.property_path = "offset_max";
                PrefabUtility::RevertPropertyOverride(_prefab_inst->GetGameObject(), ov);
            }
            ImGui::EndPopup();
        }

        if (changed && _prefab_inst)
        {
            PrefabUtility::RecordPrefabInstanceChange(_prefab_inst->GetGameObject(), _curr_component, "offset_min");
            PrefabUtility::RecordPrefabInstanceChange(_prefab_inst->GetGameObject(), _curr_component, "offset_max");
        }

        if (has_override)
        {
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetWindowPos().x, ImGui::GetItemRectMin().y) + ImVec2(4.0f, 0),
                ImVec2(ImGui::GetWindowPos().x, ImGui::GetItemRectMin().y) + ImVec2(4.0f, 0) + ImVec2(4.0f, ImGui::GetItemRectSize().y),
                ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
        }



        DisplayStack display(*this);

        _curr_property_stack.push_back("anchor_min"); display.GroupBegin(); display.Label("Anchor Min"); display.ItemBegin(true);
        changed = ImGuidk::DragVec2("##anchor_min", &c_rt->anchor_min);
        display.ItemEnd(); display.GroupEnd(changed); _curr_property_stack.pop_back();

        _curr_property_stack.push_back("anchor_max"); display.GroupBegin(); display.Label("Anchor Max"); display.ItemBegin(true);
        changed = ImGuidk::DragVec2("##anchor_max", &c_rt->anchor_max);
        display.ItemEnd(); display.GroupEnd(changed); _curr_property_stack.pop_back();

        _curr_property_stack.push_back("pivot"); display.GroupBegin(); display.Label("Pivot"); display.ItemBegin(true);
        changed = ImGuidk::DragVec2("##pivot", &c_rt->pivot, 0.01f, 0, 1.0f);
        display.ItemEnd(); display.GroupEnd(changed); _curr_property_stack.pop_back();



        // z, scale, rot
        IDE& editor = Core::GetSystem<IDE>();
        auto& c = *c_rt->GetGameObject()->Transform();

        const float item_width = ImGui::GetWindowContentRegionWidth() * 0.75f;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        ImGui::PushItemWidth(-4.0f);

        _curr_component = c.GetHandle();
        _curr_component_nth = 0;
        _curr_property_stack.push_back("position"); _curr_property_stack.push_back("z");
        display.GroupBegin(); display.Label("Pos Z"); display.ItemBegin(true);
        changed = ImGui::DragFloat("##pos_z", &c.position.z);
        display.ItemEnd(); display.GroupEnd(changed); _curr_property_stack.pop_back(); _curr_property_stack.pop_back();

        changed = false;
        _curr_property_stack.push_back("rotation");
        display.GroupBegin(); display.Label("Rotation"); display.ItemBegin(true);
        if (ImGuidk::DragQuat("##rot", &c.rotation))
        {
            changed = true;
            for (Handle<GameObject> i : editor.GetSelectedObjects().game_objects)
                i->GetComponent<Transform>()->rotation = c.rotation;
        }
        display.ItemEnd(); display.GroupEnd(changed); _curr_property_stack.pop_back();

        changed = false;
        _curr_property_stack.push_back("scale");
        display.GroupBegin(); display.Label("Scale"); display.ItemBegin(true);
        if (ImGuidk::DragVec3("##scl", &c.scale))
        {
            changed = true;
            for (Handle<GameObject> i : editor.GetSelectedObjects().game_objects)
                i->GetComponent<Transform>()->rotation = c.rotation;
        }
        display.ItemEnd(); display.GroupEnd(changed); _curr_property_stack.pop_back();
    }

    void IGE_InspectorWindow::DisplayComponentInner(Handle<RigidBody> c_rb)
    {
        auto v = c_rb->velocity();
        ImGui::DragFloat3("Velocity:", v.data());
        DisplayVal(*c_rb);
    }

    
    void IGE_InspectorWindow::DisplayComponentInner(Handle<Animator> c_anim)
    {
        ImVec2 cursorPos = ImGui::GetCursorPos();
        ImVec2 cursorPos2{};
        DisplayStack display{ *this };


        _curr_property_stack.emplace_back("layers");
        display.GroupBegin();
        auto title_pos = ImGui::GetCursorPos();
        bool tree_open = ImGui::CollapsingHeader("##animation_layers", ImGuiTreeNodeFlags_AllowItemOverlap);
        // ImGui::SetCursorPos(title_pos);
        ImGui::SameLine();
        display.Label("Layers");
        ImGui::NewLine();
        if (tree_open)
        {
            ImGui::Indent();
            for (auto& layer : c_anim->layers)
            {
                // ImGui::Separator(false);
                ImGui::PushID(&layer);

                ImGui::Text(layer.name.data());
                ImGui::SameLine();
                const auto arrow_pos = ImVec2{ ImGui::GetCurrentWindowRead()->DC.CursorPos.x,
                                       ImGui::GetCurrentWindowRead()->DC.CursorPos.y + ImGui::GetFontSize() * 0.25f };
                ImGui::RenderArrow(arrow_pos, ImGuiDir_Right, 0.7f);
                ImGui::SameLine(0.0f, ImGui::GetFrameHeight() + ImGui::GetStyle().FramePadding.x * 2);
                ImGui::Text(c_anim->CurrentStateName().data());

                ImGui::ProgressBar(layer.curr_state.normalized_time, ImVec2{ -1, 3 }, nullptr);
                ImGui::PopID();
                ImGui::Separator(false);
            }
            ImGui::Unindent();
        }
        display.GroupEnd(false);
        _curr_property_stack.pop_back();

        static char buf[50];
        const auto display_param = [&](auto& param) -> bool
        {
            using T = std::decay_t<decltype(param)>;
            bool ret_val = false;

            ImGui::PushID(&param);
            ImGui::Indent();
            strcpy_s(buf, param.name.data());
            ImGui::BeginGroup();
            const float align_widget = ImGui::GetContentRegionAvailWidth() * 0.7f;
            const float widget_size = ImGui::GetContentRegionAvailWidth() * 0.3f;

            ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.5f);
            if (ImGui::InputText("##rename_param", buf, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_EnterReturnsTrue))
            {
                ret_val |= c_anim->RenameParam<T>(param.name, buf);
            }

            ImGui::PopItemWidth();
            ImGui::SameLine(align_widget);

            auto& val = c_anim->IsPlaying() ? param.val : param.def_val;
            if constexpr (std::is_same_v <T, anim::IntParam>)
            {
                ImGui::PushItemWidth(widget_size);
                ret_val |= ImGui::DragInt("##param_input", &val) && !c_anim->IsPlaying();
                ImGui::PopItemWidth();
            }
            else if constexpr (std::is_same_v < T, anim::FloatParam>)
            {
                ImGui::PushItemWidth(widget_size);
                ret_val |= ImGui::DragFloat("##param_input", &val, 0.01f) && !c_anim->IsPlaying();
                ImGui::PopItemWidth();
            }
            else if constexpr (std::is_same_v <T, anim::BoolParam>)
            {
                ret_val |= ImGui::Checkbox("##param_input", &val) && !c_anim->IsPlaying();
            }
            else if constexpr (std::is_same_v <T, anim::TriggerParam>)
            {
                if (ImGui::RadioButton("##param_input", val))
                {
                    ret_val = true && !c_anim->IsPlaying();
                    val = !val;
                }
            }
            else
                throw("???");

            ImGui::Separator(false);

            ImGui::EndGroup();
            ImGui::Unindent();
            ImGui::PopID();

            return ret_val;
        };

        _curr_property_stack.emplace_back("parameters");
        bool params_changed = false;
        display.GroupBegin();
        title_pos = ImGui::GetCursorPos();
        tree_open = ImGui::CollapsingHeader("##animation_params", ImGuiTreeNodeFlags_AllowItemOverlap);
        // ImGui::SetCursorPos(title_pos);
        ImGui::SameLine();
        display.Label("Parameters");
        ImGui::NewLine();
        ImGui::Indent();
        if (tree_open)
        {
            if (ImGui::CollapsingHeader("Int"))
            {
                for (auto& param : c_anim->GetParamTable<anim::IntParam>())
                {
                    params_changed |= display_param(param.second);
                }
            }

            if (ImGui::CollapsingHeader("Float"))
            {
                for (auto& param : c_anim->GetParamTable<anim::FloatParam>())
                {
                    params_changed |= display_param(param.second);
                }
            }

            if (ImGui::CollapsingHeader("Bool"))
            {
                for (auto& param : c_anim->GetParamTable<anim::BoolParam>())
                {
                    params_changed |= display_param(param.second);
                }
            }

            if (ImGui::CollapsingHeader("Trigger"))
            {
                for (auto& param : c_anim->GetParamTable<anim::TriggerParam>())
                {
                    params_changed |= display_param(param.second);
                }
            }
        }
        ImGui::Unindent();
        // ImGui::Separator(false);
        display.GroupEnd(params_changed);
        _curr_property_stack.pop_back();

        ImGui::NewLine();
        if (ImGui::Button("Open Animator Window"))
        {
            Core::GetSystem<IDE>().FindWindow<IGE_AnimatorWindow>()->is_open = true;
        }
        //ImGui::SameLine(0.0f, 5.0f);

        ImGui::Text("Preview"); ImGui::SameLine();
        if (ImGui::Checkbox("##preview", &c_anim->preview_playback))
        {
            c_anim->OnPreview();
        }
        ImGui::NewLine();
    }

    
    void IGE_InspectorWindow::DisplayComponentInner(Handle<Bone> c_bone)
    {
        //Draw All your custom variables here.
        ImGui::Text("Bone Name: ");
        ImGui::SameLine();
        ImGui::Text(c_bone->bone_name.c_str());
        ImGui::Text("Bone Index: %d", c_bone->bone_index);
    }

    
    void IGE_InspectorWindow::DisplayComponentInner(Handle<TextMesh> c_font)
    {
        constexpr CustomDrawFn draw_text = [](const reflect::dynamic& val)
        {
            static std::string buf;
            buf = val.get<string>();
            if (ImGui::InputTextMultiline("", &buf))
            {
                val.get<string>() = buf;
                return true;
            }
            return false;
        };
        constexpr CustomDrawFn draw_anchor = [](const reflect::dynamic& val)
        {
            auto& anchor = val.get<TextAnchor>();
            return ImGuidk::EnumCombo("", &anchor);
        };
        constexpr CustomDrawFn draw_alignment = [](const reflect::dynamic& val)
        {
            auto& alignment = val.get<TextAlignment>();
            return ImGuidk::EnumCombo("", &alignment);
        };
        InjectDrawTable table{ { "text", draw_text }, { "anchor", draw_anchor }, { "alignment", draw_alignment } };
        DisplayVal(*c_font, &table);
    }

    
    void IGE_InspectorWindow::DisplayComponentInner(Handle<Text> c_text)
    {
        constexpr CustomDrawFn draw_text = [](const reflect::dynamic& val)
        {
            static std::string buf;
            buf = val.get<string>();
            if (ImGui::InputTextMultiline("", &buf))
            {
                val.get<string>() = buf;
                return true;
            }
            return false;
        };
        constexpr CustomDrawFn draw_alignment = [](const reflect::dynamic& val)
        {
            auto& anchor = val.get<TextAnchor>();
            return ImGuidk::EnumCombo("", &anchor);
        };
        InjectDrawTable table{ { "text", draw_text }, { "alignment", draw_alignment } };
        DisplayVal(*c_text, &table);
    }

    
    void IGE_InspectorWindow::DisplayComponentInner(Handle<AudioSource> c_audiosource)
    {
        static Handle<AudioSource> static_audiosource{};
        static_audiosource = c_audiosource;
        constexpr auto draw_audio_list = [](const reflect::dynamic& dyn) -> bool
        {
            bool changed = false;
            auto& audio_clip_list = dyn.get<vector<RscHandle<AudioClip>>>();
            if (ImGui::Button("Add AudioClip")) {
                audio_clip_list.emplace_back(RscHandle<AudioClip>());
                static_audiosource->ResizeAudioClipListData();
                changed = true;
            }

            if (!audio_clip_list.empty()) {
                ImGui::BeginChild("AudioClips", ImVec2(ImGui::GetWindowContentRegionWidth() - 20, 150), true);
                for (auto i = 0; i < audio_clip_list.size(); ++i) {
                    string txt = "[" + std::to_string(i) + "]";
                    if (ImGuidk::InputResource(txt.c_str(), &audio_clip_list[i])) {
                        //Stop playing before switching sounds!
                        changed = true;
                    }
                    ImGui::SameLine();
                    ImGui::PushID(i);
                    if (ImGui::SmallButton("X")) {
                        static_audiosource->RemoveAudioClip(i);
                        ImGui::PopID();
                        changed = true;
                        break;
                    }

                    ImGui::SameLine();
                    if (static_audiosource->IsAudioClipPlaying(i)) {
                        if (ImGui::SmallButton("||")) {
                            static_audiosource->Stop(i);
                        }
                    }
                    else {
                        if (ImGui::ArrowButton("Play", ImGuiDir_Right)) {
                            static_audiosource->Play(i);
                        }
                    }
                    ImGui::PopID();

                }

                ImGui::EndChild();
            }

            return changed;
        };

        constexpr auto draw_audio_volume = [](const reflect::dynamic& dyn) -> bool
        {
            bool changed = false;

            auto& audio_clip_volume = dyn.get<vector<float>>();

            if (!audio_clip_volume.empty()) {
                ImGui::Text("");
                ImGui::BeginChild("AudioClips_Volume", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 150), true);

                for (auto i = 0; i < audio_clip_volume.size(); ++i) {
                    //string txt = "[" + std::to_string(i) + "]";
                    ImGui::Text("[%d]", i);
                    ImGui::SameLine();
                    ImGui::PushID(i);
                    if (ImGui::DragFloat("##audioVol", &(static_audiosource->audio_clip_volume[i]), 0.01f, 0, 1, "%.2f", 1.0f)) {
                        changed = true;
                    }
                    //ImGui::Text("%.2f", static_audiosource->audio_clip_volume[i]);

                    ImGui::PopID();

                }

                ImGui::EndChild();
                ImGui::Separator();

            }

            return changed;
        };



        InjectDrawTable table{
            { "audio_clip_list", draw_audio_list },
            { "audio_clip_volume", draw_audio_volume }
        };

        DisplayVal(*c_audiosource, &table);

    }

    
    void IGE_InspectorWindow::DisplayComponentInner(Handle<ParticleSystem> c_ps)
    {
        if (c_ps->state == ParticleSystem::Playing && ImGui::Button("Pause"))
            c_ps->Pause();
        else if (c_ps->state != ParticleSystem::Playing && ImGui::Button("Play"))
            c_ps->Play();
        ImGui::SameLine();
        if (ImGui::Button("Restart"))
        {
            c_ps->Stop();
            c_ps->Play();
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
            c_ps->Stop();

        if (c_ps->state > ParticleSystem::Stopped)
        {
            ImGui::SameLine();
            ImGui::Text("%.2fs", c_ps->time);
        }

        ImGuidk::PushFont(FontType::Smaller);

        _curr_property_stack.push_back("main");
        DisplayVal(c_ps->main);
        _curr_property_stack.pop_back();

        const auto display = [&](const char* title, reflect::dynamic dyn, InjectDrawTable* inject = nullptr)
        {
            ImGui::PushID(title);

            const auto x = ImGui::GetCursorPosX();
            ImGui::SetCursorPosX(x - 1.0f);

            ImGui::GetCurrentWindow()->WorkRect.Max.x -= 5.0f; // hack
            ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetColorU32(ImGuiCol_TitleBgActive));
            const bool open = ImGui::CollapsingHeader(format_name(title).c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
            ImGui::PopStyleColor();
            ImGui::GetCurrentWindow()->WorkRect.Max.x += 5.0f; // hack

            ImGui::SameLine(x + ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Checkbox("##enabled", &dyn.get_property("enabled").value.get<bool>());

            if (open)
            {
                _curr_property_stack.push_back(title);
                DisplayVal(std::move(dyn), inject);
                _curr_property_stack.pop_back();
            }

            ImGui::PopID();
        };

        static Handle<ParticleSystem> _static_ps_handle{};
        _static_ps_handle = c_ps;
        constexpr auto draw_bursts = [](const reflect::dynamic& val) -> bool
        {
            bool changed = false;
            auto& bursts = val.get<vector<EmissionModule::Burst>>();
            if (ImGui::Button("+"))
            {
                bursts.emplace_back();
                changed = true;
            }
            if (bursts.size())
            {
                ImGui::SameLine();
                if (ImGui::Button("-"))
                {
                    bursts.pop_back();
                    changed = true;
                }
            }

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            const auto w = ImGui::CalcItemWidth();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + w * 0.1f - ImGui::CalcTextSize("Time").x * 0.5f);
            ImGui::Text("Time");
            ImGui::SameLine(w * 0.3f - ImGui::CalcTextSize("Count").x * 0.5f);
            ImGui::Text("Count");
            ImGui::SameLine(w * 0.5f - ImGui::CalcTextSize("Cycles").x * 0.5f);
            ImGui::Text("Cycles");
            ImGui::SameLine(w * 0.7f - ImGui::CalcTextSize("Interval").x * 0.5f);
            ImGui::Text("Interval");
            ImGui::SameLine(w * 0.9f - ImGui::CalcTextSize("Probability").x * 0.5f);
            ImGui::Text("Probability");

            ImGui::PushItemWidth(w * 0.2f);
            for (int i = 0; i < bursts.size(); ++i)
            {
                auto& burst = bursts[i];
                ImGui::PushID(i);
                changed |= ImGui::DragFloat("##time", &burst.time, 0.01f, 0, _static_ps_handle->main.duration);
                ImGui::SameLine(); changed |= ImGui::DragInt("##count", &burst.count, 1.0f, 1, _static_ps_handle->main.max_particles);
                ImGui::SameLine(); changed |= ImGui::DragInt("##cycles", &burst.cycles, 1.0f, 1, 32);
                ImGui::SameLine(); changed |= ImGui::DragFloat("##interval", &burst.interval, 0.01f, 0, _static_ps_handle->main.duration);
                ImGui::SameLine(); changed |= ImGui::DragFloat("##probability", &burst.probability, 0.01f, 0, 1.0f);
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::PopStyleVar();

            return changed;
        };
        InjectDrawTable inject{ { "emission/bursts", CustomDrawFn(draw_bursts) } };

        display("emission", c_ps->emission, &inject);
        display("shape", c_ps->shape);
        display("velocity_over_lifetime", c_ps->velocity_over_lifetime);
        display("color_over_lifetime", c_ps->color_over_lifetime);
        display("size_over_lifetime", c_ps->size_over_lifetime);
        display("rotation_over_lifetime", c_ps->rotation_over_lifetime);
        display("renderer", c_ps->renderer);

        ImGui::PopFont();
    }

    
    void IGE_InspectorWindow::DisplayComponentInner(Handle<ElectronView> c_ev)
    {
        switch (auto val = c_ev->owner)
        {
        case Host::ME: ImGui::Text("Owner: Me"); break;
        case Host::SERVER: ImGui::Text("Owner: SERVER"); break;
        default: ImGui::Text("Owner: %d", (int)val); break;
        }


        for (auto [name, param] : c_ev->GetParameters())
        {
            ImGui::Text(name.data());
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetStyle().IndentSpacing);
            if (param.is<vec3>())
            {
                auto v = param.get<vec3>();
                ImGui::DragFloat3(name.data(), v.data());
            }

            if (param.is<quat>())
            {
                auto euler = euler_angles{ param.get<quat>() };
                auto v = vec3{ euler.x.value, euler.y.value, euler.z.value };
                ImGui::DragFloat3(name.data(), v.data());
            }
        }
    }
}