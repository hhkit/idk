#include "pch.h"
#include "VknFrameBufferfactory.h"
#include <core/Core.h>
#include <app/Application.h>
#include <res/MetaBundle.h>
#include <vkn/VknTexture.h>
#include <vkn/VknFrameBuffer.h>
#include <vkn/VknTextureLoader.h>
#include <vkn/VulkanWin32GraphicsSystem.h>

namespace idk::vkn
{

	VknRenderTargetFactory::VknRenderTargetFactory():allocator{ *Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().Device(),Core::GetSystem<VulkanWin32GraphicsSystem>().GetVulkanHandle().View().PDevice() }
	{
		auto& vknView = Core::GetSystem<VulkanWin32GraphicsSystem>().GetVulkanHandle().View();
		fence = vknView.Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlags{} });
	}
	unique_ptr<RenderTarget> VknRenderTargetFactory::GenerateDefaultResource()
	{
		auto& rsc_manager = Core::GetResourceManager();

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
		auto ptr = rsc_manager.LoaderEmplaceResource<VknTexture>();
		auto depth_ptr = rsc_manager.LoaderEmplaceResource<VknTexture>();

		auto fb = std::make_unique<VknRenderTarget>();
		auto m = fb->GetMeta();
		m.size = Core::GetSystem<Application>().GetScreenSize();

		m.textures[RenderTarget::kColorIndex] = ptr;
		m.textures[RenderTarget::kDepthIndex] = depth_ptr;
		fb->SetMeta(m);
		fb->SetTextureCreationInfo(&allocator, *fence);

		

		//fb->AddAttachment(AttachmentType::eColor, m.size.x, m.size.y);
		//fb->AddAttachment(AttachmentType::eDepth, m.size.x, m.size.y);
		fb->rp_type = BasicRenderPasses::eRgbaColorDepth;
		auto& vknView = Core::GetSystem<VulkanWin32GraphicsSystem>().GetVulkanHandle().View();
		fb->ReattachImageViews(vknView);

		return fb;
	}
	unique_ptr<RenderTarget> VknRenderTargetFactory::Create()
	{


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


		auto fb = std::make_unique<VknRenderTarget>();
		auto m = fb->GetMeta();
		m.size = Core::GetSystem<Application>().GetScreenSize();
		auto sz = m.size;
		//m.textures.emplace_back(s_cast<RscHandle<Texture>>(ptr))->Size(m.size);
		//m.textures.emplace_back(s_cast<RscHandle<Texture>>(depth_ptr))->Size(m.size);
		fb->SetMeta(m);
		fb->SetTextureCreationInfo(&allocator, *fence);
		fb->AddAttachment(AttachmentType::eColor, m.size.x, m.size.y);
		fb->AddAttachment(AttachmentType::eDepth, m.size.x, m.size.y);
		fb->rp_type = BasicRenderPasses::eRgbaColorDepth;

		auto& vknView = Core::GetSystem<VulkanWin32GraphicsSystem>().GetVulkanHandle().View();
		fb->ReattachImageViews(vknView);

		return fb;
	}

	ResourceBundle VknFrameBufferLoader::LoadFile(PathHandle, const MetaBundle& bundle)
	{
		auto fb = Core::GetResourceManager().LoaderEmplaceResource<VknRenderTarget>(bundle.metadatas[0].guid);

		auto m = bundle.FetchMeta<RenderTarget>()->GetMeta<RenderTarget>();

		for (auto& elem : m->textures)
		{
			// ensure textures are created
			Core::GetResourceManager().Free(elem);
			Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(elem.guid);
		}
		fb->SetMeta(*m);

		return fb;
	}
}