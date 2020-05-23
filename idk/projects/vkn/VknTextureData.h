#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/ManagedVulkanObjs.h>
#include <vkn/MemoryAllocator.h>
//#include <res/ResourceHandle.h>
namespace idk::vkn
{
	struct VknTexture;
	struct VknTextureData
	{
		uvec2							size		 {};
		vk::DeviceSize					sizeOnDevice {};
		VulkanRsc<vk::Image>			image_		 { nullptr };
		vk::Format						format		 {};
		vk::ImageUsageFlags				usage		 {};
		vk::ImageAspectFlagBits			img_aspect	 ;
		vk::UniqueDeviceMemory			mem			 { nullptr };
		hlp::UniqueAlloc				mem_alloc	 {};
		VulkanRsc<vk::ImageView>		imageView	 { nullptr };
		VulkanRsc<vk::Sampler>			sampler		 { nullptr };
		opt<vk::DescriptorSet>			descriptorSet{};
		vk::ImageSubresourceRange		range		 ;
		uint32_t						mipmap_level = 1;

		string							dbg_name	 ;
													 
		vk::ImageType					image_type	 = vk::ImageType::e2D;

		uint32_t						layers       ;

		//RscHandle<VknTexture> last;

		void ApplyOnTexture(VknTexture& tex);
		vk::Image Image(bool = false)const;
		void Size(uvec2);
		void Layers(uint32_t);
		string_view Name();
		VknTextureData() = default;
		VknTextureData(VknTextureData&&) = default;
		VknTextureData& operator=(VknTextureData&&) = default;
		//~VknTextureData();
	};
}