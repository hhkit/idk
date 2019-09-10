#include "pch.h"
#pragma once
#include<vulkan/vulkan.hpp>
#include "idk.h"
#include "gfx/Texture.h"
#include <vkn/MemoryAllocator.h>
namespace idk::vkn {	
	struct VknTexture 
		: public Texture
	{
		ivec2					size{};
		vk::DeviceSize			sizeOnDevice{};
		void*					rawData{};
		string					path{""};
		vk::UniqueImage			image{nullptr};
		vk::Format				format{};
		vk::UniqueDeviceMemory  mem{nullptr};
		hlp::UniqueAlloc        mem_alloc{};
		vk::UniqueImageView     imageView{nullptr};
		vk::UniqueSampler       sampler{ nullptr };

		//Required if you want the image to be able to be used in imgui (Cast to ImTextureID)
		opt<vk::DescriptorSet>	descriptorSet{};

		VknTexture() = default;
		~VknTexture();
		//VknTexture(const VknTexture& rhs);
		VknTexture(VknTexture&& rhs);

		VknTexture& operator=(VknTexture&&);
		void Size(ivec2 new_size) override;
		virtual void* ID() const;

	private:
		void OnMetaUpdate(const TextureMeta&);
		void UpdateUV(UVMode);

	};
};