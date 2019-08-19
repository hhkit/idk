#include "pch.h"
#include "Vulkan_ImGui_Interface.h"

//#include <vulkan/VulkanWin32GraphicsSystem.h>

//Imgui
#include <imgui/imgui.h>
//Helper header from imgui for vulkan implementation
#include <editorstatic/imgui/Imgui_impl_vulkan.h>

#include <vulkan/Vulkan.h>

void idk::VIInterface_Init(Vulkan* vkObj)
{
	//IMGUI setup
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsClassic();

	Vulkan* v = vkObj;
	ImGui_ImplVulkan_InitInfo info{};
	
	
	//v->NextFrame();

	//ImGui_ImplVulkan_Init();
}
