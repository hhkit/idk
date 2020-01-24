#pragma once
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{
	struct UboManager;
	struct RenderBundle
	{
		UboManager& _ubo_manager;
		vk::CommandBuffer _cmd_buffer;
		RenderBundle(UboManager& ubo_manager, vk::CommandBuffer cmd_buffer):_ubo_manager{ubo_manager},_cmd_buffer{cmd_buffer}{}
	};
}
