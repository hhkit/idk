#include "pch.h"
#include "VulkanTextureFactory.h"
#include <vulkan/vulkan.hpp>
#include <vkn/MemoryAllocator.h>
#include <vkn/BufferHelpers.h>
#include <vkn/VknTexture.h>
#include <vkn/VulkanView.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/VknTextureLoader.h>
#include <fstream>
#include <sstream>
#include <vkn/DDSLoader.h>
namespace idk::vkn
{



	VulkanTextureFactory::VulkanTextureFactory(): _allocator{ *Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().Device(),Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().PDevice() },_dds_loader{std::make_unique<DdsLoader>()}
	{
		auto& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		
		_fence = view.Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlags{} });
	}
	DdsLoader& VulkanTextureFactory::GetDdsLoader()
	{
		// TODO: insert return statement here
		return *_dds_loader;
	}

	VulkanTextureFactory::~VulkanTextureFactory() = default; //Here for the DdsLoader's dtor

	unique_ptr<Texture> VulkanTextureFactory::GenerateDefaultResource()
	{
		//2x2 image Checkered
		uint32_t rgba[] = {0,0,0,0};// { 0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF };
		
		auto ptr = std::make_unique<VknTexture>();
		TextureLoader loader;

		TexCreateInfo tci =
		{
			2,2,
			MapFormat(TextureFormat::eRGBA32),
			vk::ImageUsageFlagBits::eSampled,
			1,
			vk::ImageAspectFlagBits::eColor,
		};

		InputTexInfo input_info
		{
			rgba,
			hlp::buffer_size(rgba),
			MapFormat(TextureFormat::eRGBA32)
		};

		loader.LoadTexture(*ptr, _allocator, *_fence, {}, tci,input_info);
		return std::move(ptr);
	}
	unique_ptr<Texture> VulkanTextureFactory::Create()
	{
		return std::make_unique<VknTexture>();
	}
	hlp::MemoryAllocator& VulkanTextureFactory::GetAllocator()
	{
		return _allocator;
	}
	vk::Fence VulkanTextureFactory::GetFence() const
	{
		return *_fence;
	}
}