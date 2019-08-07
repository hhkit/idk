#pragma once
#include <core/ISystem.h>
#include <vulkan/vulkan.hpp>

namespace idk
{
	class GraphicsSystem
		: public ISystem
	{
	public:
		void Init() override;
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