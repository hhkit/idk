#pragma once
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{
	struct UboManager;
	struct RenderBundle
	{
		vk::CommandBuffer _cmd_buffer;
		DescriptorsManager& _d_manager;
		RenderBundle(vk::CommandBuffer cmd_buffer, DescriptorsManager& d_manager) :_cmd_buffer{ cmd_buffer }, _d_manager{ d_manager }{}
	};
}
