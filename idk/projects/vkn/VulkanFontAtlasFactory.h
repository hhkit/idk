#pragma once
#include <idk.h>
#include <res/ResourceFactory.h>
#include <gfx/FontAtlas.h>
#include <vkn/MemoryAllocator.h>

namespace idk::vkn
{

	class VulkanFontAtlasFactory
		: public ResourceFactory<FontAtlas>
	{
	public:
		VulkanFontAtlasFactory();
		unique_ptr<FontAtlas> GenerateDefaultResource() override;
		unique_ptr<FontAtlas> Create() override;
	private:
		vk::UniqueFence fence;
		hlp::MemoryAllocator allocator;
	};
}