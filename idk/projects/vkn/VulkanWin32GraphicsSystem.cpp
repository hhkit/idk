#include <pch.h>
#include <iostream>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <WindowsApplication.h>

#include <vkn/VulkanState.h>

#include <core/Core.h>

//static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
//	[[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//	[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
//	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//	[[maybe_unused]] void* pUserData) {
//
//	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
//
//	return VK_FALSE;
//}
//
//VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT& pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
//	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
//	if (func != nullptr) {
//		return func(instance, &pCreateInfo, pAllocator, pDebugMessenger);
//	}
//	else {
//		return VK_ERROR_EXTENSION_NOT_PRESENT;
//	}
//}

namespace idk::vkn
{
	VulkanWin32GraphicsSystem::VulkanWin32GraphicsSystem() :  instance_{ std::make_unique<VulkanState>() }
	{
	}
	void VulkanWin32GraphicsSystem::Init()
	{
		windows_ = &Core::GetSystem<win::Windows>();
		instance_->InitVulkanEnvironment(window_info{ windows_->GetScreenSize(),windows_->GetWindowHandle(),windows_->GetInstance() });
		
	}
	void VulkanWin32GraphicsSystem::RenderBuffer()
	{
		instance_->DrawFrame();

	}
	void VulkanWin32GraphicsSystem::BeginFrame()
	{
		instance_->BeginFrame();
	}
	void VulkanWin32GraphicsSystem::EndFrame()
	{
		instance_->EndFrame();
	}
	void VulkanWin32GraphicsSystem::Shutdown()
	{
		instance_.reset();
	}
	GraphicsAPI VulkanWin32GraphicsSystem::GetAPI()
	{
		return GraphicsAPI::Vulkan;
	}
	VulkanState& VulkanWin32GraphicsSystem::GetVulkanHandle()
	{
		return *(instance_.get());
	}
	/*void VulkanWin32GraphicsSystem::createInstance()
	{
		const char* extensions[] = {
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
			VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
			VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME,
			VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
			VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
			VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
		};

		vk::ApplicationInfo appInfo; 
		appInfo.setPApplicationName("Hello Triangle");
		appInfo.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
		appInfo.setPEngineName("No Engine");
		appInfo.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
		appInfo.setApiVersion(VK_API_VERSION_1_0);
		
		vk::InstanceCreateInfo createInfo;
		createInfo.setPApplicationInfo(&appInfo);
		createInfo.setPpEnabledExtensionNames(extensions);
		createInfo.setEnabledExtensionCount((uint32_t) std::size(extensions));
		createInfo.setEnabledLayerCount(0);

		instance = vk::createInstanceUnique(createInfo);
	}*/
	/*
	void VulkanWin32GraphicsSystem::setupDebugMessenger()
	{
		vk::DebugUtilsMessengerCreateInfoEXT createInfo;
		createInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose);
		createInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
		createInfo.setPfnUserCallback(::debugCallback);

		VkDebugUtilsMessengerEXT tmp;
		CreateDebugUtilsMessengerEXT(*instance, createInfo, 0, &tmp);
		debugMessenger = VkHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic>{ tmp };
	}
	*/
}