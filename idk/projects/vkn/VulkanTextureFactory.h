#pragma once
#include <idk.h>
#include <res/ResourceFactory.h>
#include <gfx/Texture.h>
#include <vkn/MemoryAllocator.h>

namespace idk::vkn
{
	class DdsLoader;
	class VulkanTextureFactory
		: public ResourceFactory<Texture>
	{
	public:
		VulkanTextureFactory();
		VulkanTextureFactory(const VulkanTextureFactory&) = delete;
		VulkanTextureFactory(VulkanTextureFactory&&) = default;
		VulkanTextureFactory& operator=(const VulkanTextureFactory&) = delete;
		VulkanTextureFactory& operator=(VulkanTextureFactory&&) = default;
		~VulkanTextureFactory();
		void Init()override;
		unique_ptr<Texture> GenerateDefaultResource() override;
		unique_ptr<Texture> Create() override;
		hlp::MemoryAllocator& GetAllocator();
		vk::Fence GetFence()const;
		DdsLoader& GetDdsLoader();
	private:
		unique_ptr<DdsLoader> _dds_loader;
		vk::UniqueFence _fence;
		hlp::MemoryAllocator _allocator;
	};
}