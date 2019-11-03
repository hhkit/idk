#include "pch.h"
#include "ValHandler.h"
#include <vkn/utils/utils.h>
namespace idk::vkn
{
	void DoNothing() {}
	VkBool32 ValHandler::processMsg(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, [[maybe_unused]]VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
	{
		const char* prefix = "";
		if (messageSeverity == VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			prefix = "Err: ";
#if DEBUG
		hlp::cerr() << prefix << "validation layer: " << pCallbackData->pMessage << std::endl;
#else
		LOG_TO(LogPool::GFX, "%s validation layer: %s\n", prefix, pCallbackData->pMessage);// << pCallbackData->pMessage << "\n";
#endif
		if (messageSeverity == VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
 			DoNothing(), DebugBreak();
		return VK_FALSE;
	}

}