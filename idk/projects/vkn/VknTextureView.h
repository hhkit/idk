#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VknTexture.h>
namespace idk::vkn
{
	struct VknTextureView
	{
	
		VknTextureView(const VknTexture& tex) noexcept;
		VknTextureView() = default;

		VknTextureView& operator=(const VknTexture& tex) noexcept;

		vk::Image Image()const;
		vk::ImageView ImageView()const;
		vk::Sampler Sampler()const;
		uivec2 Size()const;
		vk::Format Format()const;
		vk::ImageAspectFlags ImageAspects()const;

	private:
		vk::Image            _image     {};
		vk::ImageView        _image_view{};
		vk::Sampler          _sampler   {};
		uivec2               _size      {};
		vk::Format           _format    {};
		vk::ImageAspectFlags _aspects   {};
	};
}