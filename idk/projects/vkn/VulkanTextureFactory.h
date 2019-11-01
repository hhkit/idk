#pragma once
#include <idk.h>
#include <res/ResourceFactory.h>
#include <gfx/Texture.h>
#include <vkn/MemoryAllocator.h>

namespace idk::vkn
{

	class VulkanTextureFactory
		: public ResourceFactory<Texture>
	{
	public:
		VulkanTextureFactory();
		unique_ptr<Texture> GenerateDefaultResource() override;
		unique_ptr<Texture> Create() override;
		hlp::MemoryAllocator& GetAllocator();
		vk::Fence GetFence()const;
	private:
		vk::UniqueFence _fence;
		hlp::MemoryAllocator _allocator;
	};
}