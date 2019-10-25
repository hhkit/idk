#pragma once
#include <idk.h>
#include <res\FileLoader.h>
#include <gfx\Cubemap.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{
	class CbmLoader
		: public IFileLoader
	{
	public:
		CbmLoader();
		ResourceBundle LoadFile(PathHandle handle, RscHandle<CubeMap>, const CubeMapMeta* tm = nullptr);
		ResourceBundle LoadFile(PathHandle handle) override;
		ResourceBundle LoadFile(PathHandle handle, const MetaBundle& meta) override;
	private:
		hlp::MemoryAllocator allocator;
		vk::UniqueFence fence;
	};
}