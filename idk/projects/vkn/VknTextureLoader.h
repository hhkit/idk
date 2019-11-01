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
	//enum class FilterMode
	//{
	//	eNearest,
	//	eLinear,
	//	eCubic
	//};

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
	struct InputTexInfo
	{
		const void* data{};
		size_t len{};
		vk::Format format{};
		InputTexInfo(
			const void* d=nullptr,
		size_t l=0,
			vk::Format f = {}) noexcept :data{ d }, len{ l }, format{ f }{}
	};
	struct TexCreateInfo
	{
		uint32_t width{};
		uint32_t height{};
		vk::Format internal_format;
		vk::ImageUsageFlags image_usage{};
		uint32_t mipmap_level = 1;
		vk::ImageAspectFlagBits aspect;
		vk::ImageLayout layout = vk::ImageLayout::eGeneral;
		bool read_after = false;//True if you intend to copy the memory back out for reading.
		//True if you intend to sample with a shader
		void sampled(bool will_sample)
		{
			image_usage = (will_sample) ? image_usage | vk::ImageUsageFlagBits::eSampled : (image_usage & ~vk::ImageUsageFlagBits::eSampled);
		}
	};
	TexCreateInfo ColorBufferTexInfo(uint32_t width, uint32_t height);
	TexCreateInfo DepthBufferTexInfo(uint32_t width, uint32_t height);
	class TextureLoader
	{
	public:
		//Will override TexCreateInfo's format if TextureOptions is set
		void LoadTexture(VknTexture& texture, hlp::MemoryAllocator& allocator, vk::Fence load_fence, std::optional<TextureOptions> ooptional, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info);
		void LoadTexture(VknTexture& texture, TextureFormat input_pixel_format, std::optional<TextureOptions> options, const char* rgba32, size_t len, ivec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget = false);
		void LoadTexture(VknTexture& texture, TextureFormat input_pixel_format, std::optional<TextureOptions> options, string_view rgba32, ivec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget = false);
	};

	vk::Format    MapFormat(TextureFormat tf);
	vk::Format    MapFormat(ColorFormat tf);
	TextureFormat MapFormat(vk::Format    tf);
	vk::Format    UnSrgb(vk::Format);
}