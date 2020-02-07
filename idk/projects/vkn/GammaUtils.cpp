#include "pch.h"

#include <res/ResourceHandle.inl>

#include <vkn/VknTextureLoader.h>

#include <vkn/VulkanTextureFactory.h>
namespace idk::vkn
{
	void ResizeTex(uvec2 sz, RscHandle<Texture> tx) 
	{
		auto tex = RscHandle<VknTexture>{ tx };
		TextureLoader loader;
		auto& factory = Core::GetResourceManager().GetFactory<VulkanTextureFactory>();

		TextureOptions to{tex->GetMeta()};
		TexCreateInfo tci{ tex->Size().x,tex->Size().y,tex->format,tex->usage};
		tci.aspect = vk::ImageAspectFlagBits::eColor;
		tci.layout = vk::ImageLayout::eTransferDstOptimal;
		
		loader.LoadTexture(*tex, factory.GetAllocator(), factory.GetFence(), to, tci, {}, tex.guid);
	}
	void CopyTempTex(RscHandle<Texture> src, RscHandle<Texture> target, vk::CommandBuffer cmd_buffer)
	{
		
	}

}