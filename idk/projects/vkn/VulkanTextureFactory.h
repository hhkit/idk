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
	private:
		vk::UniqueFence fence;
		hlp::MemoryAllocator allocator;
	};
}