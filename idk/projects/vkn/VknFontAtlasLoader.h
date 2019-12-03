#pragma once
#include <idk.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
#include <gfx/FontAtlas.h>

#include <vkn/VknTextureRenderMeta.h>

namespace idk::vkn
{
	class VknFontAtlas;

	struct FontAtlasOptions : FontAtlasMeta
	{
		FilterMode  min_filter{};
		FilterMode  mag_filter{};
		std::optional<CompareOp> compare_op{};
		float anisoptrophy = 1.0f;
		FontAtlasOptions() = default;
		FontAtlasOptions(const FontAtlasMeta& meta) :FontAtlasMeta{ meta } {}
	};
	struct CharacterCreateInfo {
		int x{};
		int y{};
		unsigned width, height;
	};
	struct InputFAInfo
	{
		const void* data{};
		size_t len{}; 
		//vector<CharacterCreateInfo> characterMemList{};
		//unsigned char* bitmapBuffer;
		vk::Format format{};
		InputFAInfo(
			const void* d = nullptr,
			size_t l = 0,
			vk::Format f = {}) noexcept 
			:data{ d }, 
			len{ l }, 
			format{ f }{}
	};
	struct FACreateInfo
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
	class FontAtlasLoader
	{
	public:
		//Will override TexCreateInfo's format if TextureOptions is set
		void LoadFontAtlas(VknFontAtlas& texture, hlp::MemoryAllocator& allocator, vk::Fence load_fence, std::optional<FontAtlasOptions> ooptional, const FACreateInfo& load_info, std::optional<InputFAInfo> in_info);
		void LoadFontAtlas(VknFontAtlas& texture, TextureFormat input_pixel_format, std::optional<FontAtlasOptions> options, const char* rgba32, size_t len, ivec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget = false);
		void LoadFontAtlas(VknFontAtlas& texture, TextureFormat input_pixel_format, std::optional<FontAtlasOptions> options, string_view rgba32, ivec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget = false);
	};
}