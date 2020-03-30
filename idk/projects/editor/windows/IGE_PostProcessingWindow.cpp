#include "pch.h"
#include "IGE_PostProcessingWindow.h"
#include <editor/imguidk.h>
#include <common/Transform.h>
#include <gfx/GraphicsSystem.h>
namespace idk
{
	IGE_PostProcessingWindow::IGE_PostProcessingWindow()
		:IGE_IWindow{ "Post Processing##IGE_Post_Processing",false,ImVec2{ 400,300 },ImVec2{ 450,150 } }
	{
	}
	void IGE_PostProcessingWindow::BeginWindow()
	{
	}
	void IGE_PostProcessingWindow::Update()
	{
		auto& ppe = Core::GetSystem<GraphicsSystem>().ppEffect;

		ImGui::Text("Fog Effect");
		ImGui::Separator();
		ImGui::Checkbox("Use", &ppe.useFog);
		ImGui::NewLine();
		ImGui::DragFloat3("Color", ppe.fogColor.data());
		ImGui::NewLine();
		ImGui::DragFloat("Density", &ppe.FogDensity);
		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();
		ImGui::Text("Bloom Effect");
		ImGui::Separator();
		ImGui::Checkbox("Use", &ppe.useBloom);
		ImGui::NewLine();
		ImGui::DragFloat("Brightness Threshold", ppe.threshold.data());
		ImGui::NewLine();
		ImGui::DragFloat("Blur Strength", &ppe.blurStrength);
		ImGui::NewLine();
		ImGui::DragFloat("Blue Scale", &ppe.blurScale);
		ImGui::NewLine();
	}
}