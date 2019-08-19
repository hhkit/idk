#include "pch.h"
#include "Vulkan_ImGui_Interface.h"

//#include <vulkan/VulkanWin32GraphicsSystem.h>

//Imgui
#include <vkn/Vkn.h>
#include <imgui/imgui.h>
//Helper header from imgui for vulkan implementation
#include <editorstatic/imgui/Imgui_impl_vulkan.h>


namespace idk
{
	void VIInterface_Init(VulkanState* vkObj)
	{
		//IMGUI setup
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::StyleColorsClassic();

		VulkanState* v = vkObj;
		ImGui_ImplVulkan_InitInfo info{};
		//info.Instance =
			//ImGui_ImplVulkan_Init();
	}
}