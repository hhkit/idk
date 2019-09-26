#pragma once
#include <idk.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VknTexture.h>
namespace idk::vkn
{
	struct VknTexture;



	enum class TextureFormat
	{
		eD16Unorm,
		eRGBA32,
		eBGRA32,
		eBC1,
		eBC2,
		eBC3,
		eBC4,
		eBC5,
	};
	enum class FilterMode
	{
		eNearest,
		eLinear,
		eCubic
	};

	enum class CompareOp
	{
		eNever,
		eLess,
		eEqual ,
		eLessOrEqual ,
		eGreater ,
		eNotEqual,
		eGreaterOrEqual ,
		eAlways 
	};

	struct TextureOptions : TextureMeta
	{
		FilterMode  min_filter{};
		FilterMode  mag_filter{};
		std::optional<CompareOp> compare_op{};
		float anisoptrophy = 1.0f;
		TextureOptions() = default;
		TextureOptions(const TextureMeta& meta) :TextureMeta{ meta } {}
	};
	class TextureLoader
	{
	public:
		void LoadTexture(VknTexture& texture, TextureFormat input_pixel_format, std::optional<TextureOptions> options, const char* rgba32, size_t len, ivec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget = false);
		void LoadTexture(VknTexture& texture, TextureFormat input_pixel_format, std::optional<TextureOptions> options, string_view rgba32, ivec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget = false);
	};


}