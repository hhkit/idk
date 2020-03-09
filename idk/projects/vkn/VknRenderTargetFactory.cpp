#include "pch.h"
#include "VknRenderTargetFactory.h"
#include <core/Core.h>
#include <app/Application.h>
#include <res/MetaBundle.inl>
#include <vkn/VknTexture.h>
#include <vkn/VknRenderTarget.h>
#include <vkn/VknTextureLoader.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>

namespace idk::vkn
{
	
	static const Guid default_rt_color_id = Guid{ "5B26B74B-65C5-4840-A3B5-C08DFDC11F0A" };
	static const Guid default_rt_depth_id = Guid{ "E0290658-AE4F-427F-BF21-8F52D1367B94" };

	VknRenderTargetFactory::VknRenderTargetFactory():allocator{ *Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().Device(),Core::GetSystem<VulkanWin32GraphicsSystem>().GetVulkanHandle().View().PDevice() }
	{
		auto& vknView = Core::GetSystem<VulkanWin32GraphicsSystem>().GetVulkanHandle().View();
		fence = vknView.Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlags{} });
	}
	unique_ptr<RenderTarget> VknRenderTargetFactory::GenerateDefaultResource()
	{
		auto& rsc_manager = Core::GetResourceManager();

		//Todo make these default guids
		auto ptr = rsc_manager.LoaderEmplaceResource<VknTexture>(default_rt_color_id);
		auto depth_ptr = rsc_manager.LoaderEmplaceResource<VknTexture>(default_rt_depth_id);

		auto fb = std::make_unique<VknRenderTarget>();
		auto &m = *fb;
		m.size = uvec2{ Core::GetSystem<Application>().GetScreenSize() };
		
		m.SetColorBuffer(RscHandle<Texture>{ptr      });
		m.SetDepthBuffer(RscHandle<Texture>{depth_ptr});

		return fb;
	}
	unique_ptr<RenderTarget> VknRenderTargetFactory::Create()
	{
		auto& rsc_manager = Core::GetResourceManager();

		auto ptr = rsc_manager.LoaderEmplaceResource<VknTexture>();
		auto depth_ptr = rsc_manager.LoaderEmplaceResource<VknTexture>();

		auto fb = std::make_unique<VknRenderTarget>();
		
		auto& m = *fb;
		m.Size(uvec2{ Core::GetSystem<Application>().GetScreenSize() });
		m.SetColorBuffer(RscHandle<Texture>{ptr      });
		m.SetDepthBuffer(RscHandle<Texture>{depth_ptr});
	
		return fb;
	}

	//ResourceBundle VknFrameBufferLoader::LoadFile(PathHandle path)
	//{
	//	auto loaded = base_t::LoadFile(path);
	//	auto rt = loaded.Get<VknRenderTarget>();
	//	rt->
	//	return loaded;
	//}
}