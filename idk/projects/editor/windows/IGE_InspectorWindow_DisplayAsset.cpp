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
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4.0f);

        ImGui::Text(get_asset_name(handle).c_str());

        std::visit([&](auto h)
        {
            using ResT = typename decltype(h)::Resource;
            auto path = Core::GetResourceManager().GetPath(h);

            ImGui::Text(format_name(reflect::get_type<ResT>().name()).c_str());
            ImGui::Text(string(handle.guid()).c_str());

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y);
            ImGui::Dummy(ImVec2(ImGui::GetWindowContentRegionWidth(), 4.0f));
            ImGui::EndGroup();

            ImGui::GetWindowDrawList()->ChannelsSetCurrent(0);
            ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::GetColorU32(ImGuiCol_PopupBg));
            ImGui::GetWindowDrawList()->ChannelsMerge();

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y);
            ImGui::Separator();

            DisplayAsset(h);
        }, handle);
    }

    template<>
    void IGE_InspectorWindow::DisplayAsset(RscHandle<Prefab> prefab)
    {
        auto iter = _prefab_store.find(prefab);
        if (iter == _prefab_store.end())
            iter = _prefab_store.emplace(prefab, PrefabUtility::Instantiate(prefab, *Core::GetSystem<SceneManager>().GetPrefabScene())).first;
        DisplayGameObjects({ iter->second });
        if (iter->second->GetComponent<PrefabInstance>()->overrides.size() ||
            iter->second->GetComponents().size() != prefab->data[0].components.size() + 1) // + 1 for prefab instance
            PrefabUtility::ApplyPrefabInstance(iter->second);
    }

    template<>
    void IGE_InspectorWindow::DisplayAsset(RscHandle<MaterialInstance> material)
    {
        const float item_width = ImGui::GetWindowContentRegionWidth() * item_width_ratio;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        auto graph = RscHandle<shadergraph::Graph>{ material->material };
        bool changed = false;

        for (auto& [name, u] : material->material->uniforms)
        {
            const auto y = ImGui::GetCursorPosY();

            ImGui::SetCursorPosY(y + pad_y);

            ImGui::PushID(name.c_str());

            bool has_override = material->uniforms.find(name) != material->uniforms.end();
            if (ImGui::Checkbox("##override", &has_override))
            {
                if (has_override)
                    material->uniforms[name] = material->material->uniforms[name].value;
                else
                    material->uniforms.erase(name);
            }
            ImGui::SameLine();

            ImGui::PushItemWidth(item_width);

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
                auto val = std::get<index_in_variant_v<float, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGui::DragFloat("", &val, 0.01f))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<vec2, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<vec2, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGuidk::DragVec2("", &val, 0.01f))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<vec3, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<vec3, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGuidk::DragVec3("", &val, 0.01f))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<vec4, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<vec4, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGui::DragFloat4("", val.data(), 0.01f))
                {
                    material->uniforms[name] = val;
                    changed = true;
                }
                break;
            }
            case index_in_variant_v<RscHandle<Texture>, UniformInstanceValue>:
            {
                auto val = std::get<index_in_variant_v<RscHandle<Texture>, UniformInstanceValue>>(*material->GetUniform(name));
                if (ImGuidk::InputResource("", &val))
                {
                    material->uniforms[name] = val;
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

            ImGui::PopItemWidth();
            ImGui::PopID();
        }

        if (changed)
            material->Dirty();
    }

    template<>
    void IGE_InspectorWindow::DisplayAsset(RscHandle<Material> material)
    {
        const float item_width = ImGui::GetWindowContentRegionWidth() * item_width_ratio;
        const float pad_y = ImGui::GetStyle().FramePadding.y;

        auto graph = RscHandle<shadergraph::Graph>{ material };

        if (ImGui::Button("Open Material Editor"))
            Core::GetSystem<IDE>().FindWindow<IGE_MaterialEditor>()->OpenGraph(graph);
    }

    template<>
    void IGE_InspectorWindow::DisplayAsset(RscHandle<Texture> texture)
    {
        void* id = texture->ID();
        vec2 sz = ImGui::GetContentRegionAvail();

        float aspect = texture->AspectRatio();
        if (aspect > 1.0f)
            sz.y = sz.x / aspect;
        else if (aspect < 1.0f)
            sz.x = sz.y / aspect;
        else
            sz = vec2{ ImMin(sz.x, sz.y), ImMin(sz.x, sz.y) };

        if (id)
            ImGui::Image(id, sz);
    }

	template<>
	void IGE_InspectorWindow::DisplayAsset(RscHandle<FontAtlas> texture)
	{
		void* id = texture->ID();
		vec2 sz = ImGui::GetContentRegionAvail();

		float aspect = texture->AspectRatio();
		if (aspect > 1.0f)
			sz.y = sz.x / aspect;
		else if (aspect < 1.0f)
			sz.x = sz.y / aspect;
		else
			sz = vec2{ ImMin(sz.x, sz.y), ImMin(sz.x, sz.y) };

		if (id)
			ImGui::Image(id, sz);
	}

}