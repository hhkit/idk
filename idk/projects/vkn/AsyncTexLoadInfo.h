#pragma once
#include <idk.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>

#include <vkn/VknTextureRenderMeta.h>
namespace idk::vkn
{
	struct TextureOptions
	{
		Guid guid;
		FilterMode  min_filter = FilterMode::Linear;
		FilterMode  mag_filter = FilterMode::Linear;
		FilterMode  filter_mode = FilterMode::Linear;
		UVMode      uv_mode = UVMode::Repeat;
		std::optional<CompareOp> compare_op{};
		TextureInternalFormat internal_format = TextureInternalFormat::RGBA_8;
		float anisoptrophy = 1.0f;

		bool input_is_srgb = false;

		TextureOptions() = default;
		TextureOptions(const TextureMeta& meta)
		{
			min_filter = mag_filter = filter_mode = meta.filter_mode;
			uv_mode = meta.uv_mode;
			internal_format = ToInternalFormat(meta.internal_format, meta.is_srgb);
		}
		TextureOptions(const CompiledTexture& meta);
	};

	struct InputTexInfo
	{
		const void* data{};
		size_t len{};
		vk::Format format{};
		InputTexInfo(
			const void* d = nullptr,
			size_t l = 0,
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
		vk::ImageViewType view_type = vk::ImageViewType::e2D;
		vk::ComponentMapping component_mapping{};
		vk::ImageCreateFlags image_create_bits{};
		bool read_after = false;//True if you intend to copy the memory back out for reading.
		uint32_t layers = 1;
		//True if you intend to sample with a shader
		void sampled(bool will_sample)
		{
			image_usage = mark_sampled(image_usage, will_sample);
		}
	};
	struct AsyncTexLoadInfo
	{
		shared_ptr<void> context_data;
		InputTexInfo  iti;
		TexCreateInfo tci;
		TextureOptions to;
	};
}