#pragma once
#include <gfx/Framebuffer.h>
#include <vulkan/vulkan.hpp>
#include <vkn/ManagedVulkanObjs.h>
#include <vkn/RenderPassObj.h>
namespace idk::vkn
{
	struct VknAttachment : idk::Attachment
	{
		bool override_as_depth = false;
		bool is_input_att = false;
	};
	class VknFrameBuffer: public FrameBuffer
	{
	public:
		uint32_t NumLayers()const { return static_cast<uint32_t>(FrameBuffer::NumLayers()); }
		void PrepareDraw(vk::CommandBuffer cmd_buffer);
		vk::Framebuffer GetFramebuffer()const { return *_framebuffer; }
		const RenderPassObj& GetRenderPass()const { return _renderpass; }
		void SetFramebuffer(vk::UniqueFramebuffer fb, RenderPassObj rp) { _framebuffer = std::move(fb); _renderpass = rp; }
	private:
		UniqueFramebuffer _framebuffer{};
		RenderPassObj _renderpass{};
	};
}