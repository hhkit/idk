#include "pch.h"
#include "IGE_InspectorWindow.h"

#include <editor/imguidk.h>
#include <editor/utils.h>
#include <editor/IDE.h>
#include <editor/imguidk.h>
#include <editor/windows/IGE_MaterialEditor.h>
#include <gfx/ShaderGraph.h>
#include <prefab/PrefabUtility.h>
#include <scene/SceneManager.h>
#include <IncludeComponents.h>
#include <IncludeResources.h>
#include <imgui/imgui_internal.h>
#include <reflect/reflect.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceMeta.inl>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceUtils.inl>

#include <ds/span.inl>
namespace idk
{
    static string get_asset_name(const GenericResourceHandle& handle)
    {
        return std::visit([](auto h) -> string
        {
            auto name = h->Name();
            auto path = Core::GetResourceManager().GetPath(h);
            if (!name.empty())
                return string{ name };
            else if (path)
                return string{ PathHandle{ *path }.GetStem() };
            return "";
        }, handle);
    }

    void IGE_InspectorWindow::DisplayAsset(GenericResourceHandle handle)
    {
        ImGui::GetWindowDrawList()->ChannelsSplit(2);
        ImGui::GetWindowDrawList()->ChannelsSetCurrent(1);

        ImGui::BeginGroup();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
        ImGui::Indent(8.0f);

        ImGui::Text(get_asset_name(handle).c_str());

        std::visit([&](auto h)
        {
            using ResT = typename decltype(h)::Resource;
            auto path = Core::GetResourceManager().GetPath(h);

            ImGui::Text(format_name(reflect::get_type<decltype(h)>().name()).c_str());
            ImGui::Text(string(handle.guid()).c_str());

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y);
            ImGui::Dummy(ImVec2(ImGui::GetWindowWidth(), 4.0f));

            ImGui::Unindent();
            ImGui::EndGroup();

            ImGui::GetWindowDrawList()->ChannelsSetCurrent(0);
            ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::GetColorU32(ImGuiCol_PopupBg));
            ImGui::GetWindowDrawList()->ChannelsMerge();

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y);
            ImGui::Separator();

            bool asset_displayed = DisplayAsset(h);

            if constexpr (has_tag_v<ResT, Saveable>)
            {
                try
                {

                    if (!asset_displayed && h && DisplayVal(*h))
                        h->Dirty();
					if constexpr (!ResT::autosave)
					{
						if(ImGui::Button("Save", ImVec2{60,20}))
						{
							h->Dirty();
							Core::GetResourceManager().Save(h);
						}
					}
                }
                catch (...)
                {
                    LOG_ERROR_TO(LogPool::EDIT, "Error while attempting to display Saveable asset.");
                }
            }
            else
            {
                UNREFERENCED_PARAMETER(asset_displayed);
                if constexpr (has_tag_v<ResT, MetaResource>)
                {
                    if (DisplayVal(h->GetMeta()))
                        h->DirtyMeta();
                }
            }
        }, handle);
    }
    //template<> bool IGE_InspectorWindow::DisplayAsset(RscHandle<RenderTarget> render_target)
    //{
    //    if (DisplayVal(*render_target))
    //        render_target->Dirty();
    //    return true;
    //}

    template<>
    bool IGE_InspectorWindow::DisplayAsset(RscHandle<Prefab> prefab)
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y + 2.0f);
        auto iter = _prefab_store.find(prefab);
        if (iter == _prefab_store.end())
            iter = _prefab_store.emplace(prefab, PrefabUtility::Instantiate(prefab, *Core::GetSystem<SceneManager>().GetPrefabScene())).first;
        if (!iter->second) // prefab store probably got invalidated
        {
            _prefab_store.clear();
            iter = _prefab_store.emplace(prefab, PrefabUtility::Instantiate(prefab, *Core::GetSystem<SceneManager>().GetPrefabScene())).first;
        }
        DisplayGameObjects({ iter->second });
        if (iter->second->GetComponent<PrefabInstance>()->overrides.size() > 4 ||
            iter->second->GetComponent<PrefabInstance>()->removed_components.size() ||
            iter->second->GetComponents().size() > prefab->data[0].components.size() + 1) // + 1 for prefab instance
            PrefabUtility::ApplyPrefabInstance(iter->second);
        return true;
    }

    template<>
    bool IGE_InspectorWindow::DisplayAsset(RscHandle<MaterialInstance> material_inst)
    {
        const float item_width = ImGui::GetWindowContentRegionWidth() * default_item_width_ratio;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        bool changed = false;

        ImGui::Indent(8.0f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
        ImGui::BeginChild("child");
        ImGui::PushItemWidth(-4.0f);

        ImGui::Text("Base Material");
        ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - item_width);
        changed |= ImGuidk::InputResource("##_base_mat", &material_inst->material);

        for (auto& [name, u] : material_inst->material->uniforms)
        {
            const auto y = ImGui::GetCursorPosY();

            ImGui::SetCursorPosY(y + pad_y);

            ImGui::PushID(name.c_str());

            bool has_override = material_inst->uniforms.find(name) != material_inst->uniforms.end();
            if (ImGui::Checkbox("##override", &has_override))
            {
                if (has_override)
                    material_inst->uniforms[name] = material_inst->material->uniforms[name].value;
                else
                    material_inst->uniforms.erase(name);
            }
            ImGui::SameLine();

            if (!has_override)
            {
                ImGuidk::PushDisabled();
            }

            ImGui::Text(name.c_str());
            ImGui::SetCursorPosY(y);
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - item_width);

            switch (u.value.index())
            {
            case index_in_variant_v<float, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<float, UniformInstanceValue>>(*material_inst->GetUniform(name));
                if (ImGui::DragFloat("", &val, 0.01f))
                {
                    material_inst->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<vec2, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<vec2, UniformInstanceValue>>(*material_inst->GetUniform(name));
                if (ImGuidk::DragVec2("", &val, 0.01f))
                {
                    material_inst->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<vec3, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<vec3, UniformInstanceValue>>(*material_inst->GetUniform(name));
                if (ImGuidk::DragVec3("", &val, 0.01f))
                {
                    material_inst->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<vec4, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<vec4, UniformInstanceValue>>(*material_inst->GetUniform(name));
                if (ImGui::DragFloat4("", val.data(), 0.01f))
                {
                    material_inst->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<RscHandle<Texture>, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<RscHandle<Texture>, UniformInstanceValue>>(*material_inst->GetUniform(name));
                if (ImGuidk::InputResource("", &val))
                {
                    material_inst->uniforms[name] = val;
                    changed = true;
                }
                break;
            }

            default:
                break;
            }

            if (!has_override)
            {
                ImGuidk::PopDisabled();
            }

            ImGui::PopID();
        }

        if (changed)
            material_inst->Dirty();

        ImGui::PopItemWidth();
        ImGui::EndChild();
        ImGui::Unindent();
        return true;
    }

    template<>
    bool IGE_InspectorWindow::DisplayAsset(RscHandle<Material> material)
    {
        const float item_width = ImGui::GetWindowContentRegionWidth() * default_item_width_ratio;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        auto graph = RscHandle<shadergraph::Graph>{ material };
        if (!graph)
            return true;

        ImGui::Indent(8.0f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
        ImGui::BeginChild("child");

        if (ImGui::Button("Open Material Editor"))
            Core::GetSystem<IDE>().FindWindow<IGE_MaterialEditor>()->OpenGraph(graph);

        ImGui::EndChild();
        ImGui::Unindent();
        return true;
    }

    template<>
    bool IGE_InspectorWindow::DisplayAsset(RscHandle<Texture> texture)
    {
        void* id = texture->ID();
        vec2 sz = ImGui::GetContentRegionAvail();

        float aspect = texture->AspectRatio();
        float window_aspect = sz.x / sz.y;
        if (aspect > window_aspect)
            sz.y = sz.x / aspect;
        else if (aspect < window_aspect)
            sz.x = sz.y * aspect;
        else
            sz = vec2{ ImMin(sz.x, sz.y), ImMin(sz.x, sz.y) };

        if (id)
            ImGui::Image(id, sz);
        return true;
    }

	template<>
	bool IGE_InspectorWindow::DisplayAsset(RscHandle<FontAtlas> texture)
	{
		void* id = texture->ID();
		vec2 sz = ImGui::GetContentRegionAvail();

		float aspect = texture->AspectRatio();
        float window_aspect = sz.x / sz.y;
        if (aspect > window_aspect)
            sz.y = sz.x / aspect;
        else if (aspect < window_aspect)
            sz.x = sz.y * aspect;
		else
			sz = vec2{ ImMin(sz.x, sz.y), ImMin(sz.x, sz.y) };

		if (id)
			ImGui::Image(id, sz);
        return true;
	}

}