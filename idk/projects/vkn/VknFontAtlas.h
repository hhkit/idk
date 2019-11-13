#pragma once
#include "gfx/FontAtlas.h"
#include<vulkan/vulkan.hpp>
#include "idk.h"
//#include "gfx/Texture.h"
#include <vkn/MemoryAllocator.h>
namespace idk::vkn {
	class VknFontAtlas
		:public FontAtlas
	{
		ivec2					size{};
		vk::DeviceSize			sizeOnDevice{};
		void* rawData{};
		string					path{ "" };
		vk::UniqueImage			image{ nullptr };
		vk::Format				format{};
		vk::ImageAspectFlags    img_aspect;
		vk::UniqueDeviceMemory  mem{ nullptr };
		hlp::UniqueAlloc        mem_alloc{};
		vk::UniqueImageView     imageView{ nullptr };
		vk::UniqueSampler       sampler{ nullptr };

		//Required if you want the image to be able to be used in imgui (Cast to ImTextureID)
		opt<vk::DescriptorSet>	descriptorSet{};
		vk::Sampler Sampler()const { return *sampler; }
		vk::Image Image()const { return *image; }
		vk::ImageView ImageView()const { return *imageView; }
		vk::ImageAspectFlags ImageAspects();
		VknFontAtlas() = default;
		~VknFontAtlas();
		VknFontAtlas(VknFontAtlas&& rhs) noexcept;

		VknFontAtlas& operator=(VknFontAtlas&&) noexcept;
		using FontAtlas::Size;
		void Size(ivec2 new_size) override;
		virtual void* ID() const;

	private:
		void OnMetaUpdate(const FontAtlasMeta&);
		void UpdateUV(UVMode);
	};
}