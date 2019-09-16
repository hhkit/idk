#pragma once
#include <idk.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{
	struct VknTexture;



	enum class TextureFormat
	{
		eRGBA32,
		eBGRA32,
		eBC1,
		eBC2,
		eBC3,
		eBC4,
		eBC5,
	};
	class TextureLoader
	{
	public:
		void LoadTexture(VknTexture& texture, TextureFormat pixel_format, string_view rgba32, ivec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget = false);
	};


}