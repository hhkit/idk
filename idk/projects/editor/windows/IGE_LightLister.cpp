#include "pch.h"
#include "IGE_LightLister.h"
#include <editor/imguidk.h>
#include <core/Core.h>
#include <core/GameObject.inl>
#include <editor/IDE.h>
#include <editor/commands/CommandList.h>
#include <common/Transform.h>
#include <gfx/Light.h>
#include <gfx/GraphicsSystem.h>
#include <scene/SceneManager.h>
#include <ds/span.inl>
#include <res/ResourceHandle.inl>
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
		auto scene = Core::GetSystem<SceneManager>().GetActiveScene();
		ImGui::Text("Create:"); 
		ImGui::SameLine();

		if (ImGui::Button("Point"))
		{
			auto go = scene->CreateGameObject();
			auto light = go->AddComponent<Light>();
			light->light = PointLight{};
		}
		ImGui::SameLine();

		if (ImGui::Button("Directional"))
		{
			auto go = scene->CreateGameObject();
			auto light = go->AddComponent<Light>();
			light->light = DirectionalLight{};
		}
		ImGui::SameLine();

		if (ImGui::Button("SpotLight"))
		{
			auto go = scene->CreateGameObject();
			auto light = go->AddComponent<Light>();
			light->light = SpotLight{};
		}

		ImGui::SameLine();

		if (ImGui::Button("Disable All Lights"))
		{
			for (auto& elem : Core::GetGameState().GetObjectsOfType<Light>())
				elem.enabled = false;
		}

		struct ColumnHeader
		{
			const char* label;
			float sz;
		};
		ColumnHeader headers[]=
		{
			{"On", -1},
			{"Name", 125},
			{"Col", -1},
			{"Intensity", -1},
			{"Atten", -1},
			{"Position", 250},
			{"Rotation", 250},
			{"Shadows", -1},
			{"Isolate", -1},
			{"Focus", -1}
		};

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::Columns(s_cast<int>(std::size(headers)), "", true);

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
			ImGui::Text(header);
			ImGui::NextColumn();
		}
		ImGui::Separator();
		bool isolate = false;

		auto& editor = Core::GetSystem<IDE>();

		for (auto& light : Core::GetGameState().GetObjectsOfType<Light>())
		{
			ImGui::Columns(s_cast<int>(std::size(headers)), "", true);
			ImGui::PushID((std::string{ "##LL" } +std::to_string(light.GetHandle().id)).data());
			auto go = light.GetGameObject();
			auto name = go->Name();
			auto tfm = go->Transform();

			ImGui::Checkbox("##en", &light.enabled);
			ImGui::NextColumn();

			if (ImGui::Selectable(name.data()))
			{
				editor.SelectGameObject(go);
			}
			ImGui::NextColumn();

			{
				auto color = light.GetColor();
				if (ImGui::ColorEdit3("##col", color.as_vec3.data(), ImGuiColorEditFlags_NoInputs))
					light.SetColor(color);
				ImGui::NextColumn();
			}
			{
				auto intens = light.GetLightIntensity();
				if (ImGui::DragFloat("##intens", &intens, 0.1f, 0.f, 1500.f, "%.3f", 1.1f))
					light.SetLightIntensity(intens);
				ImGui::NextColumn();
			}
			std::visit([](auto& light) 
				{
					if constexpr (!std::is_same_v<std::decay_t<decltype(light)>, DirectionalLight>)
					{
						ImGui::DragFloat("##atten", &light.attenuation_radius, 0.1f, 0.f, 1500.f, "%.3f", 1.1f);
					}
					ImGui::NextColumn();
				}, light.light);
			

			auto pos = tfm->GlobalPosition();
			if (ImGuidk::DragVec3("##tfm", &pos))
				tfm->GlobalPosition(pos);
			ImGui::NextColumn();

			auto rot = tfm->GlobalRotation();
			if (ImGuidk::DragQuat("##rot", &rot))
				tfm->GlobalRotation(rot);
			ImGui::NextColumn();

			ImGui::Checkbox("##shad", &light.casts_shadows);
			ImGui::NextColumn();

			//ImGuidk::IconCheckbox("##isol", ICON_FA_SUN, &light.isolate);
			ImGui::Checkbox("##isolate", &light.isolate);
			ImGui::NextColumn();

			if (ImGui::Button("Focus"))
			{
				editor.SelectGameObject(go);
				editor.FocusOnSelectedGameObjects();
			}
			ImGui::NextColumn();

			isolate |= light.isolate;
			ImGui::Separator();
			ImGui::PopID();
		}

		Core::GetSystem<GraphicsSystem>().isolate = isolate;
	}
}
