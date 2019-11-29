#include "pch.h"
#include "IGE_LightLister.h"
#include <editor/widgets/DragVec.h>
#include <editor/widgets/DragQuat.h>
#include <core/Core.h>
#include <core/GameObject.h>
#include <common/Transform.h>
#include <gfx/Light.h>

namespace idk
{
	IGE_LightLister::IGE_LightLister()
		:IGE_IWindow{ "Light Lister##IGE_LightLister",false,ImVec2{ 600,300 },ImVec2{ 450,150 } }
	{
	}
	void IGE_LightLister::BeginWindow()
	{
	}
	void IGE_LightLister::Update()
	{
		struct ColumnHeader
		{
			const char* label;
			float sz;
		};
		ColumnHeader headers[]=
		{
			{"On", 25},
			{"Name", 125},
			{"Color", 25},
			{"Intensity", -1},
			{"Position", -1},
			{"Rotation", -1},
			{"Shadows", 25},
		};

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::Columns(std::size(headers), "", true);

		float offset = 0.f;
		for (auto [header, size] : headers)
		{
			ImGui::SetColumnOffset(-1, offset);
			if (size >= 0)
			{
				offset += size;
			}
			else
			{
				ImVec2 textsize = ImGui::CalcTextSize(header, NULL, true);
				offset += (textsize.x + 2 * style.ItemSpacing.x);
			}
			ImGui::NextColumn();
		}

		ImGui::Separator();

		for (auto& light : Core::GetGameState().GetObjectsOfType<Light>())
		{
			auto id = std::string{ "##LL" } +std::to_string(light.GetHandle().id);
			auto go = light.GetGameObject();
			auto name = go->Name();
			auto tfm = go->Transform();

			ImGui::Checkbox((id + "en").data(), &light.enabled);
			ImGui::NextColumn();

			ImGui::Text(name.data());
			ImGui::NextColumn();

			auto color = light.GetColor();
			if (ImGui::ColorEdit3((id + "col").data(), color.as_vec3.data(), ImGuiColorEditFlags_NoInputs))
				light.SetColor(color);
			ImGui::NextColumn();

			auto pos = tfm->GlobalPosition();
			if (ImGuidk::DragVec3((id + "tfm").data(), &pos))
				tfm->GlobalPosition(pos);
			ImGui::NextColumn();

			auto rot = tfm->GlobalRotation();
			if (ImGuidk::DragQuat((id + "rot").data(), &rot))
				tfm->GlobalRotation(rot);
			ImGui::NextColumn();

			ImGui::DragFloat((id + "intens").data(), color.as_vec3.data(), 0.1, 0, 100.f, "%.3f", 1.1f);
			ImGui::NextColumn();

			ImGui::Checkbox((id + "shad").data(), &light.casts_shadows);
			ImGui::NextColumn();

			ImGui::Separator();
		}
	}
}
