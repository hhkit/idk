#include "pch.h"
#include "RenderUtil.h"

namespace idk::vkn
{
	void SetViewport(vk::CommandBuffer cmd_buffer, ivec2 vp_pos, ivec2 vp_size)
	{
		vk::Viewport vp{ s_cast<float>(vp_pos.x),s_cast<float>(vp_pos.y),s_cast<float>(vp_size.x),s_cast<float>(vp_size.y),0,1 };
		cmd_buffer.setViewport(0, vp);
	}

	void SetScissor(vk::CommandBuffer cmd_buffer, ivec2 vp_pos, ivec2 vp_size)
	{
		vk::Rect2D vp{ vk::Offset2D{vp_pos.x,vp_pos.y},vk::Extent2D{s_cast<uint32_t>(vp_size.x),s_cast<uint32_t>(vp_size.y)} };
		cmd_buffer.setScissor(0, vp);
	}
}