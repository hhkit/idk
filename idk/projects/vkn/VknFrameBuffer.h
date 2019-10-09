#pragma once
#include <gfx/Framebuffer.h>
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{
	struct VknAttachment : idk::Attachment
	{
		RscHandle<Texture> buffer;
	};
	class VknFrameBuffer: public FrameBuffer
	{
	public:
		vk::Framebuffer GetFramebuffer()const { return *_framebuffer; }
		vk::RenderPass GetRenderPass()const { return *_renderpass; }
		void SetFramebuffer(vk::UniqueFramebuffer fb) { _framebuffer = std::move(fb); }
	private:
		vk::UniqueFramebuffer _framebuffer;
		vk::UniqueRenderPass _renderpass;
	};
}