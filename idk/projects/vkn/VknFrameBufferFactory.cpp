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

		//Todo make these default guids
		auto ptr = rsc_manager.LoaderEmplaceResource<VknTexture>();
		auto depth_ptr = rsc_manager.LoaderEmplaceResource<VknTexture>();

		auto fb = std::make_unique<VknRenderTarget>();
		auto m = fb->GetMeta();
		m.size = Core::GetSystem<Application>().GetScreenSize();

		m.textures[RenderTarget::kColorIndex] = ptr;
		m.textures[RenderTarget::kDepthIndex] = depth_ptr;
		//Render target will initialize itself onmetaupdate
		fb->SetMeta(m);
		return fb;
	}
	unique_ptr<RenderTarget> VknRenderTargetFactory::Create()
	{
		auto& rsc_manager = Core::GetResourceManager();

		auto ptr = rsc_manager.LoaderEmplaceResource<VknTexture>();
		auto depth_ptr = rsc_manager.LoaderEmplaceResource<VknTexture>();

		auto fb = std::make_unique<VknRenderTarget>();
		auto m = fb->GetMeta();
		m.size = Core::GetSystem<Application>().GetScreenSize();
		m.textures[RenderTarget::kColorIndex] = ptr;
		m.textures[RenderTarget::kDepthIndex] = depth_ptr;
		//Render target will initialize itself onmetaupdate
		fb->SetMeta(m);
	
		return fb;
	}

	ResourceBundle VknFrameBufferLoader::LoadFile(PathHandle, const MetaBundle& bundle)
	{
		auto fb = Core::GetResourceManager().LoaderEmplaceResource<VknRenderTarget>(bundle.metadatas[0].guid);

		auto m = bundle.FetchMeta<RenderTarget>()->GetMeta<RenderTarget>();
		fb->SetMeta(*m);

		return fb;
	}
}