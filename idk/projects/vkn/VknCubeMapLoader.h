#pragma once
#include <idk.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VknCubemap.h>

#include <vkn/VknTextureRenderMeta.h>

namespace idk::vkn
{
	struct VknCubemap;

	struct CubemapOptions : CubeMapMeta
	{
		FilterMode  min_filter{};
		FilterMode  mag_filter{};
		std::optional<CompareOp> compare_op{};
		float anisoptrophy = 1.0f;
		CubemapOptions() = default;
		CubemapOptions(const CubeMapMeta& meta) :CubeMapMeta{ meta } {}
	};
	struct InputCMInfo
	{
		const void* data{};
		size_t len{};
		vector<size_t> stride{0};
		size_t mem_size{};
		vk::Format format{};
		InputCMInfo(
			const void* d = nullptr,
			size_t l = 0,
			vector<size_t> s = { 0 },
			size_t ms = 0,
			vk::Format f = {}) noexcept :data{ d }, len{ l }, stride{ s }, mem_size{ms},format{ f }{}
	};
	struct CMCreateInfo
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
	CMCreateInfo CMColorBufferTexInfo(uint32_t width, uint32_t height);
	//CMCreateInfo DepthBufferTexInfo(uint32_t width, uint32_t height);
	class CubemapLoader
	{
	public:
		//Will override TexCreateInfo's format if TextureOptions is set
		void LoadCubemap(VknCubemap& texture, hlp::MemoryAllocator& allocator, vk::Fence load_fence, std::optional<CubemapOptions> ooptional, const CMCreateInfo& load_info, std::optional<InputCMInfo> in_info);
		void LoadCubemap(VknCubemap& texture, TextureFormat input_pixel_format, std::optional<CubemapOptions> options, const char* rgba32, size_t len, uivec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget = false);
		void LoadCubemap(VknCubemap& texture, TextureFormat input_pixel_format, std::optional<CubemapOptions> options, string_view rgba32, uivec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence, bool isRenderTarget = false);
	};
}