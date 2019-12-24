#include "pch.h"
#include "VulkanCubemapFactory.h"
#include <vulkan/vulkan.hpp>
#include <vkn/MemoryAllocator.h>
#include <vkn/BufferHelpers.h>
#include <vkn/VknCubemap.h>
#include <vkn/VulkanView.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/VknCubemapLoader.h>
#include <fstream>
#include <sstream>
#include <res/ResourceManager.inl>
namespace idk::vkn
{



	VulkanCubemapFactory::VulkanCubemapFactory() : allocator{ *Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().Device(),Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().PDevice() }
	{
		auto& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();

		fence = view.Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlags{} });
	}

	unique_ptr<CubeMap> VulkanCubemapFactory::GenerateDefaultResource()
	{
		//2x2 image Checkered
		uint32_t rgba[] = {
			0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF,
			0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF,
			0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF,
			0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF,
			0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF,
			0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF,
		};
		
		auto ptr = std::make_unique<VknCubemap>();
		ptr->texture = Core::GetResourceManager().LoaderEmplaceResource<VknTexture>();
		CubemapLoader loader;
		loader.LoadCubemap(*ptr, TextureFormat::eRGBA32, {}, string_view{ r_cast<const char*>(rgba),hlp::buffer_size(rgba) }, ivec2{ 2,2 }, allocator, *fence);
		return std::move(ptr);
	}
	unique_ptr<CubeMap> VulkanCubemapFactory::Create()
	{
		return std::make_unique<VknCubemap>();
	}
}