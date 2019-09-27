#pragma once
#include <idk.h>
#include <res\FileLoader.h>
#include <gfx\Texture.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{
	class PngLoader
		: public IFileLoader
	{
	public:
		PngLoader();
		ResourceBundle LoadFile(PathHandle handle,RscHandle<Texture>,const TextureMeta* tm=nullptr) ;
		ResourceBundle LoadFile(PathHandle handle) override;
		ResourceBundle LoadFile(PathHandle handle, const MetaBundle& meta) override;
	private:
		hlp::MemoryAllocator allocator;
		vk::UniqueFence fence;
	};
}