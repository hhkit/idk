#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>

namespace idk::vkn
{
	struct ValHandler
	{
		virtual VkBool32 processMsg(
			[[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
			[[maybe_unused]] const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);
	};
}