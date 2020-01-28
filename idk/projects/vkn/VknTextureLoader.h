#pragma once
#include <idk.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VknTexture.h>

#include <vkn/VknTextureRenderMeta.h>

namespace idk::vkn
{
	struct VknTexture;

	struct TextureOptions
	{
		FilterMode  min_filter = FilterMode::Linear;
		FilterMode  mag_filter = FilterMode::Linear;
		FilterMode  filter_mode = FilterMode::Linear;
		UVMode      uv_mode = UVMode::Repeat;
		std::optional<CompareOp> compare_op{};
		TextureInternalFormat internal_format;
		float anisoptrophy = 1.0f;

		TextureOptions() = default;
		TextureOptions(const TextureMeta& meta) 
		{
			min_filter = mag_filter = filter_mode = meta.filter_mode;
			uv_mode = meta.uv_mode;
			internal_format = ToInternalFormat(meta.internal_format, meta.is_srgb);
		}
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
	inline vk::ImageUsageFlags mark_sampled(vk::ImageUsageFlags flags, bool is_sampled) noexcept
	{
		return (is_sampled) ? flags | vk::ImageUsageFlagBits::eSampled : (flags & ~vk::ImageUsageFlagBits::eSampled);
	}
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
		uint32_t layers = 1;
		//True if you intend to sample with a shader
		void sampled(bool will_sample)
		{
			image_usage = mark_sampled(image_usage,will_sample);
		}
	};
	TexCreateInfo ColorBufferTexInfo(uint32_t width, uint32_t height);
	TexCreateInfo DepthBufferTexInfo(uint32_t width, uint32_t height);
	class TextureLoader
	{
	public:
		//Will override TexCreateInfo's format if TextureOptions is set
		void LoadTexture(VknTexture& texture, hlp::MemoryAllocator& allocator, vk::Fence load_fence, std::optional<TextureOptions> ooptional, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info);
		//TODO: Deprecate these 2 functions below usage the one above instead
		void LoadTexture(VknTexture& texture, TextureFormat input_pixel_format, std::optional<TextureOptions> options, const char* rgba32, size_t len, uvec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget = false);
		void LoadTexture(VknTexture& texture, TextureFormat input_pixel_format, std::optional<TextureOptions> options, string_view rgba32, uvec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget = false);
	};
}