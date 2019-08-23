#include "pch.h"
#include "Vulkan_ImGui_Interface.h"

//#include <vulkan/VulkanWin32GraphicsSystem.h>

//Imgui
#include <vkn/VulkanState.h>
#include <vkn/VulkanView.h>
#include <vkn/BufferHelpers.h>
#include <imgui/imgui.h>

//Helper header from imgui for vulkan implementation
#include <editorstatic/imgui/Imgui_impl_vulkan.h>


namespace idk
{
	void VIInterface_Init(vkn::VulkanState* vkObj)
	{
		//IMGUI setup
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::StyleColorsClassic();

		vkn::VulkanState* v = vkObj;
		//ImGui_ImplVulkan_InitInfo info{};
		//info.Instance =
			//ImGui_ImplVulkan_Init();

		idk::vkn::VulkanView& vknViews = vkObj->View();

		ImGui_ImplVulkan_InitInfo info{};
		info.Instance = *vknViews.Instance();
		info.PhysicalDevice = vknViews.PDevice();
		info.Device = *vknViews.Device();
		
		info.QueueFamily = *vknViews.QueueFamily().graphics_family;
		info.Queue = vknViews.GraphicsQueue();

		info.PipelineCache = VK_NULL_HANDLE;

		vk::DescriptorPoolSize pSizes[] =
		{
			
			{ vk::DescriptorType::eSampler, 1000 },
			{ vk::DescriptorType::eCombinedImageSampler, 1000 },
			{ vk::DescriptorType::eSampledImage, 1000 },
			{ vk::DescriptorType::eStorageImage, 1000 },
			{ vk::DescriptorType::eUniformTexelBuffer, 1000 },
			{ vk::DescriptorType::eStorageTexelBuffer, 1000 },
			{ vk::DescriptorType::eUniformBuffer, 1000 },
			{ vk::DescriptorType::eStorageBuffer, 1000 },
			{ vk::DescriptorType::eUniformBufferDynamic, 1000 },
			{ vk::DescriptorType::eStorageBufferDynamic, 1000 },
			{ vk::DescriptorType::eInputAttachment, 1000 }
		};
		vk::DescriptorPoolCreateInfo pInfo{
			vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
			1000 * vkn::hlp::arr_count(pSizes),
			vkn::hlp::arr_count(pSizes),
			pSizes
		};
		//vknViews.Device()->createDescriptorPoolUnique()
		//
		//info.DescriptorPool = VK_NULL_HANDLE;
		//info.Allocator = vknViews.

	}
}