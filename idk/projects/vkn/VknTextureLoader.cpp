#include "pch.h"
#include "VknTextureLoader.h"
#include <vkn/MemoryAllocator.h>
#include <vkn/BufferHelpers.h>
#include <vkn/VknTexture.h>
#include <vkn/VulkanView.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
namespace idk::vkn
{
	vk::UniqueImageView CreateImageView2D(vk::Device device, vk::Image image, vk::Format format);
	std::pair<vk::UniqueImage, hlp::UniqueAlloc> LoadTexture(hlp::MemoryAllocator& allocator, vk::Fence fence, const void* data, uint32_t width, uint32_t height, size_t len, vk::Format format);

	vk::Format    MapFormat(TextureFormat tf);
	TextureFormat MapFormat(vk::Format    tf);

	void TextureLoader::LoadTexture(VknTexture& texture, TextureFormat pixel_format, string_view rgba32, ivec2 size, hlp::MemoryAllocator& allocator, vk::Fence load_fence)
	{
		auto& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		//2x2 image Checkered
		const void* rgba = std::data(rgba32);
		auto format = MapFormat(pixel_format);
		auto ptr = &texture;
		auto&& [image, alloc] = vkn::LoadTexture(allocator, load_fence, rgba, size.x, size.y, rgba32.length(), format);
		ptr->image = std::move(image);
		ptr->mem_alloc = std::move(alloc);
		//TODO set up Samplers and Image Views

		auto device = *view.Device();
		ptr->imageView = CreateImageView2D(device, *ptr->image, format);

		vk::SamplerCreateInfo sampler_info
		{
			vk::SamplerCreateFlags{},
			vk::Filter::eNearest,
			vk::Filter::eNearest,
			vk::SamplerMipmapMode::eNearest,
			vk::SamplerAddressMode::eRepeat,
			vk::SamplerAddressMode::eRepeat,
			vk::SamplerAddressMode::eRepeat,
			0.0f,
			VK_TRUE,
			1.0f,
			VK_FALSE,//Used for percentage close filtering
			vk::CompareOp::eAlways,
			0.0f,0.0f,
			vk::BorderColor::eFloatOpaqueBlack,
			VK_FALSE

		};
		ptr->sampler = device.createSamplerUnique(sampler_info);

	}

	template<typename K, typename V>
	hash_table<V, K> ReverseMap(const hash_table<K, V>& map);

	hash_table<TextureFormat, vk::Format> FormatMap();

	void TransitionImageLayout(vk::CommandBuffer cmd_buffer,
		vk::AccessFlags src_flags, vk::PipelineStageFlags src_stage,
		vk::AccessFlags dst_flags, vk::PipelineStageFlags dst_stage,
		vk::ImageLayout original_layout, vk::ImageLayout target, vk::Image image, std::optional<vk::ImageSubresourceRange> range = {})
	{
		if (!range)
			range =
			vk::ImageSubresourceRange
		{
			vk::ImageAspectFlagBits::eColor,0,1,0,1
		};
		vk::ImageMemoryBarrier barrier
		{
			src_flags,dst_flags,original_layout,target,VK_QUEUE_FAMILY_IGNORED,VK_QUEUE_FAMILY_IGNORED,image,*range
		};
		cmd_buffer.pipelineBarrier(src_stage, dst_stage, {}, nullptr, nullptr, barrier, vk::DispatchLoaderDefault{});
	}




	vk::UniqueImageView CreateImageView2D(vk::Device device, vk::Image image, vk::Format format)
	{
		vk::ImageViewCreateInfo viewInfo{
			vk::ImageViewCreateFlags{},
			image						   ,//image                           
			vk::ImageViewType::e2D		   ,//viewType                        
			format	   ,//format                          
			vk::ComponentMapping{},
			vk::ImageSubresourceRange
		{
			vk::ImageAspectFlagBits::eColor,//aspectMask     
			0							   ,//baseMipLevel   
			1							   ,//levelCount     
			0							   ,//baseArrayLayer 
			1							   	//layerCount     
		}
		};
		return device.createImageViewUnique(viewInfo);

	}

	std::pair<vk::UniqueImage, hlp::UniqueAlloc> LoadTexture(hlp::MemoryAllocator& allocator, vk::Fence fence, const void* data, uint32_t width, uint32_t height, size_t len, vk::Format format)
	{
		VulkanView& view = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View();
		vk::PhysicalDevice pd = view.PDevice();
		vk::Device device = *view.Device();
		auto ucmd_buffer = hlp::BeginSingleTimeCBufferCmd(device, *view.Commandpool());
		auto cmd_buffer = *ucmd_buffer;
		bool is_render_target = false;

		size_t num_bytes = len;

		vk::ImageUsageFlags attachment_type = vk::ImageUsageFlagBits::eColorAttachment;
		vk::ImageLayout     attachment_layout = vk::ImageLayout::eColorAttachmentOptimal;
		std::optional<vk::ImageSubresourceRange> range{};

		vk::ImageCreateInfo imageInfo{};
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1; //1 texel deep, can't put 0, otherwise it'll be an array of 0 2D textures
		imageInfo.mipLevels = 1; //Currently no mipmapping
		imageInfo.arrayLayers = 1;
		imageInfo.format = format; //Unsigned normalized so that it can still be interpreted as a float later
		imageInfo.tiling = vk::ImageTiling::eOptimal; //We don't intend on reading from it afterwards
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = vk::ImageUsageFlagBits::eSampled | ((is_render_target) ? attachment_type : vk::ImageUsageFlagBits::eTransferDst); //Image needs to be transfered to and Sampled from
		imageInfo.sharingMode = vk::SharingMode::eExclusive; //Only graphics queue needs this.
		imageInfo.samples = vk::SampleCountFlagBits::e1; //Multisampling

		vk::UniqueImage image = device.createImageUnique(imageInfo, nullptr, vk::DispatchLoaderDefault{});
		auto alloc = allocator.Allocate(*image, vk::MemoryPropertyFlagBits::eDeviceLocal);
		device.bindImageMemory(*image, alloc->Memory(), alloc->Offset(), vk::DispatchLoaderDefault{});

		auto&& [stagingBuffer, stagingMemory] = hlp::CreateAllocBindBuffer(pd, device, num_bytes, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, vk::DispatchLoaderDefault{});

		hlp::MapMemory(device, *stagingMemory, 0, data, num_bytes, vk::DispatchLoaderDefault{});
		vk::AccessFlags src_flags = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eShaderRead;
		vk::AccessFlags dst_flags = vk::AccessFlagBits::eTransferWrite;
		vk::PipelineStageFlags shader_flags = vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader;// | vk::PipelineStageFlagBits::eTessellationControlShader | vk::PipelineStageFlagBits::eTessellationEvaluationShader;
		vk::PipelineStageFlags src_stages = shader_flags;
		vk::PipelineStageFlags dst_stages = vk::PipelineStageFlagBits::eTransfer;
		vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal;
		if (is_render_target)
		{
			src_flags |= vk::AccessFlagBits::eColorAttachmentRead;
			src_stages |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dst_flags |= vk::AccessFlagBits::eColorAttachmentWrite;
			dst_stages |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
			layout = attachment_layout;
		}
		TransitionImageLayout(cmd_buffer, src_flags, shader_flags, dst_flags, dst_stages, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, *image);

		//Copy data from buffer to image
		vk::BufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};
		cmd_buffer.copyBufferToImage(*stagingBuffer, *image, vk::ImageLayout::eTransferDstOptimal, region, vk::DispatchLoaderDefault{});

		;

		TransitionImageLayout(cmd_buffer, src_flags, shader_flags, dst_flags, dst_stages, vk::ImageLayout::eTransferDstOptimal, layout, *image);

		device.resetFences(fence);
		hlp::EndSingleTimeCbufferCmd(cmd_buffer, view.GraphicsQueue(), false, fence);
		uint64_t wait_for_milli_seconds = 3;
		uint64_t wait_for_micro_seconds = wait_for_milli_seconds * 1000;
		uint64_t wait_for_nano_seconds = wait_for_micro_seconds * 1000;
		device.waitForFences(fence, VK_TRUE, wait_for_nano_seconds);
		return std::pair<vk::UniqueImage, hlp::UniqueAlloc>{std::move(image), std::move(alloc)};

	}

	template<typename K, typename V>
	hash_table<V, K> ReverseMap(const hash_table<K, V>& map)
	{
		hash_table<V, K> result;
		for (auto& [k, v] : map)
		{
			result.emplace(v, k);
		}
		return result;
	}
	hash_table<TextureFormat, vk::Format> FormatMap()
	{
		return hash_table<TextureFormat, vk::Format>
		{
			{TextureFormat::eRGBA32, vk::Format::eR8G8B8A8Unorm},
			{ TextureFormat::eBC1,vk::Format::eBc1RgbaUnormBlock },
			{ TextureFormat::eBC2,vk::Format::eBc2UnormBlock },
			{ TextureFormat::eBC3,vk::Format::eBc3UnormBlock },
			{ TextureFormat::eBC4,vk::Format::eBc4UnormBlock },
			{ TextureFormat::eBC5,vk::Format::eBc5UnormBlock },

		};

	}

	vk::Format MapFormat(TextureFormat tf)
	{
		static const auto map = FormatMap();
		return map.find(tf)->second;
	}
	TextureFormat MapFormat(vk::Format tf)
	{
		static const auto map = ReverseMap(FormatMap());
		return map.find(tf)->second;
	}
}