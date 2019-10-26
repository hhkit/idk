#pragma once
#include <gfx/Framebuffer.h>
#include <vulkan/vulkan.hpp>
#include <vkn/ManagedVulkanObjs.h>
namespace idk::vkn
{
	struct VknAttachment : idk::Attachment
	{
	};
	class VknFrameBuffer: public FrameBuffer
	{
	public:
		void PrepareDraw(vk::CommandBuffer cmd_buffer);
		vk::Framebuffer GetFramebuffer()const { return *_framebuffer; }
		vk::RenderPass GetRenderPass()const { return _renderpass; }
		void SetFramebuffer(vk::UniqueFramebuffer fb, vk::RenderPass rp) { _framebuffer = std::move(fb); _renderpass = rp; }
	private:
		UniqueFramebuffer _framebuffer{};
		vk::RenderPass _renderpass{};
	};
}