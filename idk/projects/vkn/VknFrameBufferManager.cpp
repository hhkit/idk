#include "pch.h"
#include "VknFrameBufferManager.h"
#include <gfx/RenderTarget.h>
#include <vkn/VknFrameBuffer.h>
#include <vkn/VknTexture.h>

#include <vulkan/vulkan.hpp>

#include <iostream>

#include <core/Core.h>
#include <gfx/GraphicsSystem.h>
#include <vkn/VulkanWin32GraphicsSystem.h>


namespace idk::vkn
{
	VknFrameBufferManager::VknFrameBufferManager()
	{
		//auto& vknView = Core::GetSystem<VulkanWin32GraphicsSystem>().GetVulkanHandle().View();
		//auto sz = vknView.GetWindowsInfo().size;

		//vk::ImageCreateInfo imageInfo = {};


		//imageInfo.imageType = vk::ImageType::e2D;
		//imageInfo.extent.width = s_cast<uint32_t>(sz.x);
		//imageInfo.extent.height = s_cast<uint32_t>(sz.y);
		//imageInfo.extent.depth = 1;
		//imageInfo.mipLevels = 1;
		//imageInfo.arrayLayers = 1;
		//imageInfo.format = vk::Format::eR8G8B8A8Unorm;
		//imageInfo.tiling = vk::ImageTiling::eOptimal;
		//imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		//imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
		//imageInfo.samples = vk::SampleCountFlagBits::e1;
		//imageInfo.sharingMode = vk::SharingMode::eExclusive;

		//VknImageData imgd = {vknView.Device()->createImageUnique(imageInfo,nullptr,vknView.Dispatcher())};

		//imgd.size = vec2{ sz };

		//vk::ImageViewCreateInfo createInfo{
		//			vk::ImageViewCreateFlags{},
		//			*imgd.image,
		//			vk::ImageViewType::e2D,
		//			vknView.Swapchain().surface_format.format,
		//			vk::ComponentMapping{},
		//			vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor,0,1,0,1 }
		//};

		//vknView.Device()->createImageView(createInfo, nullptr, vknView.Dispatcher());

		////Gen a framebuffer
		//curr_framebuffer = VknFrameBuffer{ std::move(imgd),vknView };
	}

	VknFrameBufferManager::VknFrameBufferManager(VknFrameBufferManager&& rhs)
		:curr_framebuffer{std::move(rhs.curr_framebuffer)}
	{

	}
	VknFrameBufferManager& VknFrameBufferManager::operator=(VknFrameBufferManager&& rhs)
	{
		std::swap(curr_framebuffer,rhs.curr_framebuffer);

		//Same thing
		return *this;
	}
	VknFrameBufferManager::~VknFrameBufferManager()
	{
		//glDeleteFramebuffers(1, &_fbo_id);
	}
	void VknFrameBufferManager::SetRenderTarget(RscHandle<VknFrameBuffer> target)
	{
		//Set render target is a impt function
		curr_framebuffer = std::move(*target);
	}
	void VknFrameBufferManager::ResetFramebuffer()
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	unique_ptr<VknFrameBuffer> VknFrameBufferManager::Framebuffer()
	{
		//return RscHandle<VknFrameBuffer>(curr_framebuffer);

		//ARGH FK I WILL REMEMBER THIS
		return std::make_unique<VknFrameBuffer>(std::move(curr_framebuffer));
	}
}
