#pragma once
#include <gfx/GraphicsSystem.h>
#define VK_USE_PLATFORM_WIN32_KHR
#ifdef free
#undef free
#endif
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_win32.h>

namespace idk
{
	class VulkanWin32GraphicsSystem
		: public GraphicsSystem
	{
	public:
		void Init() override ;
		void Shutdown() override;
	private:
		template<typename T, typename D = vk::DispatchLoaderStatic>
		using VkHandle = vk::UniqueHandle<T, D>;

		VkHandle<vk::Instance> instance;
		VkHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> debugMessenger;

		void createInstance();
		void setupDebugMessenger();
	};
}