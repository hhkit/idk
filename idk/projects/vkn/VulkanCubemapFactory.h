#pragma once
#include <idk.h>
#include <res/ResourceFactory.h>
#include <gfx/Cubemap.h>
#include <vkn/MemoryAllocator.h>

namespace idk::vkn
{

	class VulkanCubemapFactory
		: public ResourceFactory<CubeMap>
	{
	public:
		VulkanCubemapFactory();
		unique_ptr<CubeMap> GenerateDefaultResource() override;
		unique_ptr<CubeMap> Create() override;
	private:
		vk::UniqueFence fence;
		hlp::MemoryAllocator allocator;
	};
}