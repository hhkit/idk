#pragma once
#pragma once
#include<vulkan/vulkan.hpp>
#include "idk.h"
#include "gfx/Cubemap.h"
#include <vkn/MemoryAllocator.h>
namespace idk::vkn {

	struct VknCubemap
		: public CubeMap
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
		VknCubemap() = default;
		~VknCubemap();
		//VknTexture(const VknTexture& rhs);
		VknCubemap(VknCubemap&& rhs) noexcept;

		VknCubemap& operator=(VknCubemap&&) noexcept;
		void Size(ivec2 new_size) override;
		virtual void* ID() const;

		void SetConvoluted(const RscHandle<VknCubemap>&);

	private:
		RscHandle<VknCubemap> _convoluted;
		void OnMetaUpdate(const CubeMapMeta&);
		void UpdateUV(CMUVMode);

	};
};