#pragma once
#include<vulkan/vulkan.hpp>
#include "idk.h"
#include "gfx/Texture.h"
#include <vkn/MemoryAllocator.h>
#include <meta/stl_hack.h>
namespace idk::vkn {

	struct VknTexture
		: public Texture
	{
		uvec2					size{};
		vk::DeviceSize			sizeOnDevice{};
		void* rawData{};
		string					path{ "" };
		vk::UniqueImage			image_{ nullptr };
		vk::Format				format{};
		vk::ImageUsageFlags     usage{};
		vk::ImageAspectFlags    img_aspect;
		vk::UniqueDeviceMemory  mem{ nullptr };
		hlp::UniqueAlloc        mem_alloc{};
		vk::UniqueImageView     imageView{ nullptr };
		vk::UniqueSampler       sampler{ nullptr };
		opt<vk::DescriptorSet>	descriptorSet{};

		VknTexture() = default;
		~VknTexture();
		VknTexture(const VknTexture & rhs) = delete;
		VknTexture(VknTexture && rhs) noexcept;

		VknTexture& operator=(VknTexture&&) noexcept;
		vk::Sampler Sampler()const { return *sampler; }
		vk::Image Image()const { return *image_; }
		vk::ImageView ImageView()const { return *imageView; }
		vk::ImageAspectFlags ImageAspects()const;
		uint32_t& Layers(uint32_t layers)noexcept;
		uint32_t Layers()const noexcept;
		using Texture::Size;
		uvec2 Size(uvec2 new_size) override;
		//Required if you want the image to be able to be used in imgui (Cast to ImTextureID)
		void* ID() const override;

	private:
		uint32_t                _layers;
		void OnMetaUpdate(const TextureMeta&);
		void UpdateUV(UVMode);

	};
};

MARK_NON_COPY_CTORABLE(idk::vkn::VknTexture)