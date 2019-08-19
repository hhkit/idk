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
	class Windows;
}
namespace idk::vkn
{
	class VulkanState;

	class VulkanWin32GraphicsSystem
		: public GraphicsSystem
	{
	public:
		VulkanWin32GraphicsSystem(Windows& windows_app);
		void Init() override ;

		void BeginFrame() ;
		void EndFrame() ;
		void Shutdown() override;
		void RenderBuffer() override;
		VulkanState& Instance() { return *instance_; }

		VulkanState& GetVulkanHandle();
	private:
		std::unique_ptr<VulkanState> instance_;
		Windows* windows_;
		template<typename T, typename D = vk::DispatchLoaderStatic>
		using VkHandle = vk::UniqueHandle<T, D>;
	};
}