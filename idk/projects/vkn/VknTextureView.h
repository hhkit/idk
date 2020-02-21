#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{
	struct VknTexture;
	struct VknCubemap;
	struct VknTextureView
	{
	
		VknTextureView(const VknTexture& tex) noexcept;
		VknTextureView(const VknCubemap& tex) noexcept;
		VknTextureView() = default;

		VknTextureView& operator=(const VknTexture& tex) noexcept;
		VknTextureView& operator=(const VknCubemap& tex) noexcept;

		vk::Image                 Image       ()const noexcept;
		vk::ImageView             ImageView   ()const noexcept;
		vk::Sampler               Sampler     ()const noexcept;
		uvec2                     Size        ()const noexcept;
		vk::Format                Format      ()const noexcept;
		vk::ImageAspectFlags      ImageAspects()const noexcept;
		uint32_t                  Layers      ()const noexcept;
		vk::ImageSubresourceRange FullRange   ()const noexcept;

	private:
		vk::Image                 _image     {};
		vk::ImageView             _image_view{};
		vk::Sampler               _sampler   {};
		uvec2                     _size      {};
		vk::Format                _format    {};
		vk::ImageAspectFlags      _aspects   {};
		uint32_t                  _layers    {};
		vk::ImageSubresourceRange _range     {};
	};
}