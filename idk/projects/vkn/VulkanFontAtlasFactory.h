#pragma once
#include <idk.h>
#include <res/ResourceFactory.h>
#include <gfx/FontAtlas.h>
#include <vkn/MemoryAllocator.h>
#include <vkn/VulkanIttfLoader.h>

namespace idk::vkn
{

	class VulkanFontAtlasFactory
		: public ResourceFactory<FontAtlas>
	{
	public:
		VulkanFontAtlasFactory();
		unique_ptr<FontAtlas> GenerateDefaultResource() override;
		unique_ptr<FontAtlas> Create() override;

		IttfLoader& GetIttfLoader();
	private:
		unique_ptr<IttfLoader> _ittf_loader;
		vk::UniqueFence fence;
		hlp::MemoryAllocator allocator;
	};
}