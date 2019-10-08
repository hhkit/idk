#include "pch.h"
#include "VknFrameBuffer.h"
#include <core/Core.h>
#include <vkn/VknTexture.h>
#include <vkn/VulkanWin32GraphicsSystem.h>

#include <vkn/VknTextureLoader.h>

#include <vkn/VknFrameBufferFactory.h>

namespace idk::vkn
{
	VulkanView& View()
	{
		return Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
	}

	void VknRenderTarget::OnMetaUpdate(const Metadata& new_meta)
	{
		for (auto& elem : meta.textures)
			Core::GetResourceManager().Free(elem);
		//TODO get these from somewhere persistent.
		auto& factory = Core::GetResourceManager().GetFactory<VknRenderTargetFactory>();
		hlp::MemoryAllocator& alloc = factory.GetAllocator();
		vk::Fence fence = factory.GetFence();

		//Reload the textures with the new configuration.
		TextureLoader loader;
		

		Core::GetResourceManager().Free(new_meta.textures[kColorIndex]);
		Core::GetResourceManager().Free(new_meta.textures[kDepthIndex]);
		auto color_tex = Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(new_meta.textures[kColorIndex].guid);
		loader.LoadTexture(*color_tex, TextureFormat::eBGRA32, {}, nullptr, 0, new_meta.size, alloc, fence, true);
		auto depth_tex = Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(new_meta.textures[kDepthIndex].guid);
		loader.LoadTexture(*depth_tex, TextureFormat::eD16Unorm, {}, nullptr, 0, new_meta.size, alloc, fence, true);

		{
			VulkanView& vknView = View();
			const vk::ImageView image_views[] = {color_tex->ImageView(),depth_tex->ImageView()};

			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.renderPass = vknView.BasicRenderPass(rp_type);
			framebufferInfo.attachmentCount = hlp::arr_count(image_views);
			framebufferInfo.pAttachments = std::data(image_views);
			framebufferInfo.width  = s_cast<uint32_t>(new_meta.size.x);
			framebufferInfo.height = s_cast<uint32_t>(new_meta.size.y);
			framebufferInfo.layers = 1;

			size = new_meta.size;

			buffer = vknView.Device()->createFramebufferUnique(framebufferInfo, nullptr, vknView.Dispatcher());

		}
	}
	
	void TransitionTexture(vk::CommandBuffer cmd_buffer, vk::ImageLayout type, VknTexture& tex)
	{
		hlp::TransitionImageLayout(true, cmd_buffer, View().GraphicsQueue(), *tex.image, tex.format, vk::ImageLayout::eUndefined, type);
	}

	void VknRenderTarget::PrepareDraw(vk::CommandBuffer& cmd_buffer)
	{
		TransitionTexture(cmd_buffer, vk::ImageLayout::eColorAttachmentOptimal       , meta.textures[kColorIndex].as<VknTexture>());
		TransitionTexture(cmd_buffer, vk::ImageLayout::eDepthStencilAttachmentOptimal, meta.textures[kDepthIndex].as<VknTexture>());
	}

	vk::RenderPass VknRenderTarget::GetRenderPass() const
	{
		return View().BasicRenderPass(rp_type);
	}

	vk::Framebuffer VknRenderTarget::Buffer()
	{
		return *buffer;
	}

	vk::Semaphore VknRenderTarget::ReadySignal()
	{
		return *ready_semaphore;
	}


	/*GLuint VknFrameBuffer::DepthBuffer() const
	{
		return depthbuffer;
	}*/

}