#include "pch.h"
#pragma once
#include<vulkan/vulkan.hpp>
#include "idk.h"

namespace idk::vkn {	
	struct idkTexture {
		ivec2					size{};
		vk::DeviceSize			sizeOnDevice{};
		void*					rawData{};
		string					path{""};
		vk::UniqueImage			vknData{nullptr};
		vk::Format				format{};
		vk::UniqueDeviceMemory  mem{nullptr};
		vk::UniqueImageView     imageView{nullptr};
		vk::UniqueSampler       sampler{ nullptr };

		idkTexture() = default;
		~idkTexture();
		//idkTexture(const idkTexture& rhs);
		idkTexture(idkTexture&& rhs);
	};
};