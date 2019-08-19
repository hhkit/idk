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
	class Vulkan;
	class Windows;

	class VulkanWin32GraphicsSystem
		: public GraphicsSystem
	{
	public:
		VulkanWin32GraphicsSystem(Windows& windows_app);
		void Init() override ;
		void Draw() override ;

		void BeginFrame() override;
		void EndFrame() override;
		void Shutdown() override;
		Vulkan& Instance() { return *instance_; }

		Vulkan& GetVulkanHandle();
	private:
		std::unique_ptr<Vulkan> instance_;
		Windows* windows_;
		template<typename T, typename D = vk::DispatchLoaderStatic>
		using VkHandle = vk::UniqueHandle<T, D>;

		VkHandle<vk::Instance> instance;
		VkHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> debugMessenger;

		//void createInstance();
		//void setupDebugMessenger();
	};
}