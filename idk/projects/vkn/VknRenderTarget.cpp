#include "pch.h"
#include "VknRenderTarget.h"
#include <core/Core.h>
#include <vkn/VknTexture.h>
#include <vkn/VulkanWin32GraphicsSystem.h>

#include <vkn/VknTextureLoader.h>

#include <vkn/VknRenderTargetFactory.h>

namespace idk::vkn
{
	VulkanView& View()
	{
		return Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
	}

	void VknRenderTarget::OnFinalize()
	{
		for (auto& elem : Textures())
		{
			if (elem == RscHandle<Texture>{})
				elem = RscHandle<Texture>{Core::GetResourceManager().LoaderEmplaceResource<VknTexture>()};
			Core::GetResourceManager().Release(elem);
		}
		//TODO get these from somewhere persistent.
		auto& factory = Core::GetResourceManager().GetFactory<VknRenderTargetFactory>();
		hlp::MemoryAllocator& alloc = factory.GetAllocator();
		vk::Fence fence = factory.GetFence();

		//Reload the textures with the new configuration.
		TextureLoader loader;


		//TODO store a framebuffer instead.
		auto color_texture = Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(GetColorBuffer().guid);
		auto ctci = ColorBufferTexInfo(s_cast<uint32_t>(size.x), s_cast<uint32_t>(size.y));
		loader.LoadTexture(*color_texture, alloc, fence, {}, ctci, {});
		auto depth_texture = Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(GetDepthBuffer().guid);
		TexCreateInfo dtci = DepthBufferTexInfo(s_cast<uint32_t>(size.x), s_cast<uint32_t>(size.y));
		loader.LoadTexture(*depth_texture, alloc, fence, {}, dtci, {});

		{ 
			VulkanView& vknView = View();
			const vk::ImageView image_views[] = {color_texture->ImageView(),depth_texture->ImageView()};

			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.renderPass = vknView.BasicRenderPass(rp_type);
			framebufferInfo.attachmentCount = hlp::arr_count(image_views);
			framebufferInfo.pAttachments = std::data(image_views);
			framebufferInfo.width  = s_cast<uint32_t>(size.x);
			framebufferInfo.height = s_cast<uint32_t>(size.y);
			framebufferInfo.layers = 1;

			buffer = vknView.Device()->createFramebufferUnique(framebufferInfo, nullptr, vknView.Dispatcher());

		}
	}
	
	void TransitionTexture(vk::CommandBuffer cmd_buffer, vk::ImageLayout type, VknTexture& tex)
	{
		hlp::TransitionImageLayout(true, cmd_buffer, View().GraphicsQueue(), tex.Image(), tex.format, vk::ImageLayout::eUndefined, type);
	}

	void VknRenderTarget::PrepareDraw(vk::CommandBuffer& cmd_buffer)
	{
		TransitionTexture(cmd_buffer, vk::ImageLayout::eColorAttachmentOptimal       , GetColorBuffer().as<VknTexture>());
		TransitionTexture(cmd_buffer, vk::ImageLayout::eDepthStencilAttachmentOptimal, GetDepthBuffer().as<VknTexture>());
	}

	vk::RenderPass VknRenderTarget::GetRenderPass(bool clear_col , bool clear_depth ) const
	{
		
		return View().BasicRenderPass(rp_type,clear_col,clear_depth);
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