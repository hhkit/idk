#include "pch.h"
#include "PresentationSignals.h"
#include "vkn/VulkanView.h"
#include "vkn/utils/SwapchainInfo.h"

#include <vkn/VulkanState.h>

#include "TriBuffer.h"

namespace idk::vkn {
	TriBuffer::TriBuffer(VulkanView& view,const bool& cImage)
		:images{ }, image_views{}, pSignals{}, mems{*view.Device(),view.PDevice()}
	{
		pSignals.resize(view.MaxFrameInFlight());
		for (auto& elem : pSignals)
			elem.Init(view);
	}
	/*
	TriBuffer::TriBuffer(vector<vk::Image> img, vector<vk::UniqueImageView> imgView,vector<hlp::MemoryAllocator> m, VulkanView& view)
		:images{ img }, image_views{ std::move(imgView) }, mems{std::move(m)}, pSignals{}
	{
		pSignals.resize(view.MaxFrameInFlight());
		
		for(auto& elem: pSignals)
			elem.Init(view);
	}
	*/
	TriBuffer::TriBuffer(TriBuffer&& rhs)
		:images{ std::move(rhs.images) }, image_views{ std::move(rhs.image_views) }, pSignals{ std::move(rhs.pSignals) }, mems{std::move(rhs.mems)}
	{
	}
	TriBuffer& TriBuffer::operator=(TriBuffer&& rhs)
	{
		// TODO: insert return statement here
		std::swap(images,rhs.images);
		std::swap(image_views,rhs.image_views);
		std::swap(mems,rhs.mems);
		std::swap(pSignals,rhs.pSignals);

		return *this;
	}
	TriBuffer::~TriBuffer()
	{
		images.clear();
		image_views.clear();
		pSignals.clear();
	}
	void TriBuffer::CreateImageViewWithCurrImgs(VulkanView& view)
	{
		image_views.clear();
		//for (size_t i = 0; i < images.size(); ++i)
		for (auto& image : images)
		{
			vk::ImageViewCreateInfo createInfo{
				vk::ImageViewCreateFlags{},
				image,
				vk::ImageViewType::e2D,
				view.vulkan().surfaceFormat.format,
				vk::ComponentMapping{},
				vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor,0,1,0,1 }
			};
			image_views.emplace_back(view.Device()->createImageViewUnique(createInfo, nullptr, view.Dispatcher()));
		}
	}
	void TriBuffer::CreatePresentationSignals(VulkanView& view)
	{
		pSignals.resize(view.MaxFrameInFlight());
		for (auto& elem : pSignals)
			elem.Init(view);
	}
	void TriBuffer::CreateImagePool(VulkanView& view)
	{
		images.clear();
		for (unsigned int i = 0; i < view.vulkan().imageCount; ++i)
		{
			//auto ptr = std::make_unique<VknTexture>();
			//auto&& [image, alloc] = TextureLoader::LoadTexture(allocator, load_fence, rgba, size.x, size.y, len, format, isRenderTarget);
			images.emplace_back(vk::Image{});

			view.vulkan().createImage(
				view.vulkan().extent.width,
				view.vulkan().extent.height,
				view.vulkan().surfaceFormat.format,
				vk::ImageTiling::eOptimal,
				vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eInputAttachment,
				vk::MemoryPropertyFlagBits::eDeviceLocal,
				images[images.size() - 1],
				mems
			);

		}
		CreateImageViewWithCurrImgs(view);
	}
};