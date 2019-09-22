#include "pch.h"
#include "VknFrameBufferfactory.h"
#include <core/Core.h>
#include <app/Application.h>
#include <vkn/VknTexture.h>
#include <vkn/VknFrameBuffer.h>
#include <vkn/VknTextureLoader.h>
#include <vkn/VulkanWin32GraphicsSystem.h>

namespace idk::vkn
{

	VknFrameBufferFactory::VknFrameBufferFactory():allocator{ *Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().Device(),Core::GetSystem<VulkanWin32GraphicsSystem>().GetVulkanHandle().View().PDevice() }
	{
		auto& vknView = Core::GetSystem<VulkanWin32GraphicsSystem>().GetVulkanHandle().View();
		fence = vknView.Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlags{} });
	}
	unique_ptr<RenderTarget> VknFrameBufferFactory::GenerateDefaultResource()
	{


		/*vk::ImageCreateInfo imageInfo = {};


		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width = s_cast<uint32_t>(sz.x);
		imageInfo.extent.height = s_cast<uint32_t>(sz.y);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = vk::Format::eR8G8B8A8Unorm;
		imageInfo.tiling = vk::ImageTiling::eOptimal;
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
		imageInfo.samples = vk::SampleCountFlagBits::e1;
		imageInfo.sharingMode = vk::SharingMode::eExclusive;

		vk::Image image = vknView.Device()->createImage(imageInfo,nullptr,vknView.Dispatcher());*/




		//Todo make this a default guid
		auto ptr = Core::GetResourceManager().Emplace<VknTexture>();
		TextureLoader loader;


		//imgd.size = vec2{ sz };

		/*vk::ImageViewCreateInfo createInfo{
					vk::ImageViewCreateFlags{},
					*ptr->image,
					vk::ImageViewType::e2D,
					vknView.Swapchain().surface_format.format,
					vk::ComponentMapping{},
					vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor,0,1,0,1 }
		};

		vknView.Device()->createImageView(createInfo, nullptr, vknView.Dispatcher());*/


		auto fb = std::make_unique<VknFrameBuffer>();
		auto m = fb->GetMeta();
		m.size = Core::GetSystem<Application>().GetScreenSize();
		auto sz = m.size;
		m.textures.emplace_back(s_cast<RscHandle<Texture>>(ptr))->Size(m.size);
		fb->SetMeta(m);
		loader.LoadTexture(*ptr, TextureFormat::eBGRA32, {}, nullptr, s_cast<size_t>(4 * sz.x * sz.y), ivec2{ sz }, allocator, *fence, true);

		auto& vknView = Core::GetSystem<VulkanWin32GraphicsSystem>().GetVulkanHandle().View();
		fb->ReattachImageViews(vknView);

		return fb;
	}
	unique_ptr<RenderTarget> VknFrameBufferFactory::Create()
	{
		auto fb = std::make_unique<VknFrameBuffer>();
		auto m = fb->GetMeta();
		m.size = ivec2{ 512, 512 };
		m.textures.emplace_back(Core::GetResourceManager().Create<VknTexture>())->Size(m.size);
		fb->SetMeta(m);
		return fb;
	}
	unique_ptr<RenderTarget> VknFrameBufferFactory::Create(PathHandle fh)
	{
		return unique_ptr<RenderTarget>();
	}

	unique_ptr<RenderTarget> VknFrameBufferFactory::Create(PathHandle filepath, const RenderTarget::Metadata& m)
	{
		auto fb = std::make_unique<VknFrameBuffer>();

		for (auto& elem : m.textures)
		{
			// ensure textures are created
			Core::GetResourceManager().Free(elem);
			Core::GetResourceManager().Emplace<VknTexture>(elem.guid);
		}
		fb->SetMeta(m);

		return fb;
	}
}