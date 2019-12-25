#pragma once
#include <vulkan/vulkan.hpp>
#include <idk.h>
namespace idk::vkn::dbg
{
	//Don't use string_view, vulkan takes in a c_string so we can't tell it how long the string really is.
	void AddLabel(vk::CommandBuffer cmd_buffer, const char* label, const color& col = idk::color{ 0,0,1,1 });
	void BeginLabel(vk::CommandBuffer cmd_buffer, const char*, const color& col = idk::color{ 0,0,1,1 });
	void EndLabel(vk::CommandBuffer cmd_buffer);
}
