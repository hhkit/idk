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

#include <gfx/ColorGrade.h>

#include <res/ResourceHandle.inl>
#include <res/ResourceManager.inl>

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

	void VulkanTextureFactory::Init()
	{
		constexpr Guid color_grade_id = {0xFADAu,0xFADAu,0xFADAu ,0xFADAu };
		auto color_grade_data = GenerateRgbaDefaultColorGradeTexData();

		auto ptr = Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(color_grade_id);

		TextureLoader loader;

		TexCreateInfo tci =
		{
			color_grade_data.dimensions.x,color_grade_data.dimensions.y,
			MapFormat(TextureFormat::eRGBA32),
			vk::ImageUsageFlagBits::eSampled,
			1,
			vk::ImageAspectFlagBits::eColor,
		};

		InputTexInfo input_info
		{
			color_grade_data.data.data(),
			hlp::buffer_size(color_grade_data.data),
			MapFormat(TextureFormat::eRGBA32)
		};
		ptr->Name(" Default Color Grade LUT");
		TextureOptions to{};
		to.input_is_srgb = false;
		to.uv_mode = UVMode::Clamp;
		loader.LoadTexture(*ptr, _allocator, *_fence, to, tci, input_info);
		
	}

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