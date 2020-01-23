#pragma once
#include <vkn/VulkanPipeline.h>
#include <gfx/GraphicsSystem.h>

namespace idk::vkn
{
	VulkanView& View();
	void SetViewport(vk::CommandBuffer cmd_buffer, ivec2 vp_pos, uivec2 vp_size);
	void SetScissor(vk::CommandBuffer cmd_buffer, ivec2 vp_pos, uivec2 vp_size);
}