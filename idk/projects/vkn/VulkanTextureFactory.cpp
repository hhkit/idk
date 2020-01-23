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
namespace idk::vkn
{



	VulkanTextureFactory::VulkanTextureFactory(): _allocator{ *Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().Device(),Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().PDevice() }
	{
		auto& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		
		_fence = view.Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlags{} });
	}

	unique_ptr<Texture> VulkanTextureFactory::GenerateDefaultResource()
	{
		//2x2 image Checkered
		uint32_t rgba[] = {0,0,0,0};// { 0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF };
		
		auto ptr = std::make_unique<VknTexture>();
		TextureLoader loader;
		loader.LoadTexture(*ptr, TextureFormat::eRGBA32, {}, string_view{ r_cast<const char*>(rgba),hlp::buffer_size(rgba) }, uivec2{ 2,2 }, _allocator, *_fence);
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