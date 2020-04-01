#pragma once
#include <idk.h>
#include <res\FileLoader.h>
#include <vkn/VknFontAtlasLoader.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{
	class IttfLoader
		: public IFileLoader
	{
	public:
		IttfLoader();
		void LoadFontAtlas(VknFontAtlas&, string_view file, const FontAtlasOptions& fao);
		//ResourceBundle LoadFile(PathHandle handle, RscHandle<FontAtlas>, const FontAtlasMeta* tm = nullptr);
		ResourceBundle LoadFile(PathHandle handle, const MetaBundle& meta) override;
	private:
		hlp::MemoryAllocator allocator;
		vk::UniqueFence fence;
	};
}