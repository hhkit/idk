#include "pch.h"
#include "VknFrameBuffer.h"
#include <vkn/VknTexture.h>
namespace idk::vkn 
{
	void TransitionTexture(vk::CommandBuffer cmd_buffer, vk::ImageLayout type, VknTexture& tex);
	void VknFrameBuffer::PrepareDraw(vk::CommandBuffer cmd_buffer)
	{
		for (auto& attachment : attachments)
		{
			TransitionTexture(cmd_buffer, vk::ImageLayout::eColorAttachmentOptimal, attachment->buffer.as<VknTexture>());
		}
		TransitionTexture(cmd_buffer, vk::ImageLayout::eDepthStencilAttachmentOptimal, depth_attachment->buffer.as<VknTexture>());
	}
}
