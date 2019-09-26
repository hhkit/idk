#include "pch.h"
#include "ValHandler.h"
#include <vkn/utils/utils.h>
namespace idk::vkn
{
	VkBool32 ValHandler::processMsg(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, [[maybe_unused]]VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
	{
		if (messageSeverity == VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			hlp::cerr() << "Err: ";
		hlp::cerr() << "validation layer: " << pCallbackData->pMessage << "\n";

		if (messageSeverity == VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			5;
		return VK_FALSE;
	}

}