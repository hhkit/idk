#include "pch.h"
#include "VulkanTextureFactory.h"
#include <vulkan/vulkan.hpp>
#include <vkn/MemoryAllocator.h>
#include <vkn/BufferHelpers.h>
#include <vkn/VknTexture.h>
namespace idk::vkn
{

	//Refer to https://www.khronos.org/registry/vulkan/specs/1.0/html/chap6.html#synchronization-access-types for access flags
	void TransitionImageLayout(vk::CommandBuffer cmd_buffer,
		vk::AccessFlags src_flags, vk::PipelineStageFlags src_stage,
		vk::AccessFlags dst_flags, vk::PipelineStageFlags dst_stage,
		vk::ImageLayout target, vk::Image image, std::optional<vk::ImageSubresourceRange> range ={})
	{
		if (!range)
			range =
			vk::ImageSubresourceRange
			{
				vk::ImageAspectFlagBits::eColor,0,1,0,1
			};
		vk::ImageMemoryBarrier barrier
		{
			src_flags,dst_flags,vk::ImageLayout::eUndefined,target,VK_QUEUE_FAMILY_IGNORED,VK_QUEUE_FAMILY_IGNORED,image,*range
		};
		cmd_buffer.pipelineBarrier(src_stage, dst_stage, {}, nullptr, nullptr, barrier, vk::DispatchLoaderDefault{});
	}


	std::pair<vk::UniqueImage,hlp::UniqueAlloc> LoadTexture(vk::PhysicalDevice pd,vk::Device device,hlp::MemoryAllocator& allocator,const void* rgba, uint32_t width, uint32_t height)
	{
		vk::DeviceMemory image_memory;
		bool is_render_target = false;

		constexpr size_t rgba_size = 4;
		size_t num_bytes = s_cast<size_t>(width) * height * rgba_size;

		vk::ImageUsageFlags attachment_type = vk::ImageUsageFlagBits::eColorAttachment;
		vk::ImageLayout     attachment_layout = vk::ImageLayout::eColorAttachmentOptimal;
		std::optional<vk::ImageSubresourceRange> range{};

		vk::ImageCreateInfo imageInfo{};
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width  = static_cast<uint32_t>(width );
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1; //1 texel deep, can't put 0, otherwise it'll be an array of 0 2D textures
		imageInfo.mipLevels = 1; //Currently no mipmapping
		imageInfo.arrayLayers = 1;
		imageInfo.format = vk::Format::eR8G8B8A8Unorm; //Unsigned normalized so that it can still be interpreted as a float later
		imageInfo.tiling = vk::ImageTiling::eOptimal; //We don't intend on reading from it afterwards
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = vk::ImageUsageFlagBits::eSampled | ((is_render_target)? attachment_type: vk::ImageUsageFlagBits::eTransferDst); //Image needs to be transfered to and Sampled from
		imageInfo.sharingMode = vk::SharingMode::eExclusive; //Only graphics queue needs this.
		imageInfo.samples = vk::SampleCountFlagBits::e1; //Multisampling

		vk::UniqueImage image = device.createImageUnique(imageInfo, nullptr, vk::DispatchLoaderDefault{});
		auto alloc =allocator.Allocate(*image, vk::MemoryPropertyFlagBits::eDeviceLocal);
		device.bindImageMemory(*image, alloc->Memory(), alloc->Offset(), vk::DispatchLoaderDefault{});

		auto&&[stagingBuffer,stagingMemory] =hlp::CreateAllocBindBuffer(pd, device, num_bytes, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, vk::DispatchLoaderDefault{});
		
		hlp::MapMemory(device, *stagingMemory, 0, rgba, num_bytes, vk::DispatchLoaderDefault{});
		vk::CommandBuffer cmd_buffer;
		vk::AccessFlags src_flags = vk::AccessFlagBits::eMemoryRead| vk::AccessFlagBits::eShaderRead;
		vk::AccessFlags dst_flags = vk::AccessFlagBits::eTransferWrite;
		vk::PipelineStageFlags shader_flags = vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eTessellationControlShader | vk::PipelineStageFlagBits::eTessellationEvaluationShader;
		vk::PipelineStageFlags src_stages = shader_flags;
		vk::PipelineStageFlags dst_stages = vk::PipelineStageFlagBits::eTransfer;
		vk::ImageLayout layout = vk::ImageLayout::eTransferDstOptimal;
		if (is_render_target)
		{
			src_flags |= vk::AccessFlagBits::eColorAttachmentRead;
			src_stages|= vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dst_flags |= vk::AccessFlagBits::eColorAttachmentWrite;
			dst_stages |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
			layout = attachment_layout;
		}


		TransitionImageLayout(cmd_buffer, src_flags, shader_flags, dst_flags, dst_stages, layout, *image);

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
		cmd_buffer.copyBufferToImage(*stagingBuffer, *image, layout, region, vk::DispatchLoaderDefault{});

		layout = vk::ImageLayout::eShaderReadOnlyOptimal;

		TransitionImageLayout(cmd_buffer, src_flags, shader_flags, dst_flags, dst_stages, layout, *image);

		return std::pair<vk::UniqueImage, hlp::UniqueAlloc>{std::move(image),std::move(alloc)};

	}



	unique_ptr<Texture> idk::vkn::VulkanTextureFactory::GenerateDefaultResource()
	{
		//2x2 image Checkered
		uint32_t rgba[] = { 0x000000FF, 0xFFFFFFFF, 0xFFFFFFFF ,0x000000FF };
		vk::Device d;
		vk::PhysicalDevice pd;
		hlp::MemoryAllocator allocator{ d,pd };

		auto ptr = std::make_unique<VknTexture>();
		auto&& [image, alloc] = LoadTexture(pd, d, allocator, rgba, 2, 2);
		ptr->vknData = std::move(image);
		ptr->mem_alloc = std::move(alloc);
		//TODO set up Samplers and Image Views

		return std::move(ptr);
	}
	unique_ptr<Texture> VulkanTextureFactory::Create()
	{
		return unique_ptr<Texture>();
	}
	unique_ptr<Texture> VulkanTextureFactory::Create(FileHandle filepath)
	{
		return unique_ptr<Texture>();
	}
	unique_ptr<Texture> VulkanTextureFactory::Create(FileHandle filepath, const Texture::Metadata&)
	{
		return unique_ptr<Texture>();
	}
}