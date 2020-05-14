#pragma once
#include <idk.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VknTexture.h>

#include <vkn/VknTextureRenderMeta.h>
#include <vkn/FencePool.h>
#include <parallel/ThreadPool.h>
namespace idk
{
	struct CompiledTexture;
}

namespace idk::vkn
{
	struct VknTexture;

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
		vk::ImageViewType view_type = vk::ImageViewType::e2D;
		vk::ComponentMapping component_mapping{};
		vk::ImageCreateFlags image_create_bits{};
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
	TexCreateInfo StencilBufferTexInfo(uint32_t width, uint32_t height);
	TexCreateInfo Depth3DBufferTexInfo(uint32_t width, uint32_t height);
	class TextureLoader
	{
	public:
		//Will override TexCreateInfo's format if TextureOptions is set
		struct SubmissionObjs
		{
			std::optional<vk::CommandBuffer> cmd_buffer;
			vk::Fence load_fence;
			bool end_and_submit = true;
		};
		std::optional< std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory>> LoadTexture(SubmissionObjs sub,VknTexture& texture, hlp::MemoryAllocator& allocator,std::optional<TextureOptions> ooptional, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid = {});
		void LoadTexture(VknTexture& texture, hlp::MemoryAllocator& allocator, vk::Fence load_fence,std::optional<TextureOptions> ooptional, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid = {});
		mt::ThreadPool::Future<void> LoadTextureAsync(VknTexture& texture, hlp::MemoryAllocator& allocator, FencePool& load_fence, CmdBufferPool& cmd_buffers, std::optional<TextureOptions> ooptional, TexCreateInfo load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid = {});

		hlp::SimpleLock lock;
		int abc = 0;
	};

	struct ImageViewInfo
	{
		uint32_t base_mip_level = 0;
		uint32_t level_count = 1;
		uint32_t base_array_layer = 0;
		uint32_t array_layer_count = 1;
		vk::ImageViewType type = vk::ImageViewType::e2D;
		vk::ComponentMapping component_mapping{};
	};

	vk::UniqueImageView CreateImageView2D(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspect, ImageViewInfo = {});
}