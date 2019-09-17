#include "pch.h"
#pragma once
#include<vulkan/vulkan.hpp>
#include "idk.h"
namespace idk::vkn {
	struct VknImageData
	{
		vk::UniqueImage			image{ nullptr };
		vk::Format				format{};
		vk::UniqueImageView     imageView{ nullptr };

		vec2					size{};

		VknImageData() = default;
		VknImageData(vk::UniqueImage);

		virtual ~VknImageData();
		//VknTexture(const VknTexture& rhs);
		VknImageData(VknImageData&& rhs);

		VknImageData& operator=(VknImageData&&);

		/*VknImageData(const VknImageData& rhs);

		VknImageData& operator=(const VknImageData&);*/
	};
};