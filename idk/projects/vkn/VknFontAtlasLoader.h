#pragma once
#include <idk.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
#include <gfx/FontAtlas.h>
#include <res/FileLoader.h>

namespace idk::vkn
{
	class VknFontAtlasLoader
		: public IFileLoader
	{
	public:
		VknFontAtlasLoader();
		ResourceBundle LoadFile(PathHandle handle, RscHandle<FontAtlas>, const FontAtlasMeta* tm = nullptr);
		ResourceBundle LoadFile(PathHandle handle, const MetaBundle& meta) override;
	private:
		hlp::MemoryAllocator allocator;
		vk::UniqueFence fence;
	};
}