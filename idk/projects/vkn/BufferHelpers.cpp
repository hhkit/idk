#include "pch.h"
#include <vkn/BufferHelpers.h>
#include "VulkanDebugRenderer.h"
#include "vkn/VknTexture.h"
#include <optional>
namespace  idk::vkn::hlp
{
uint32_t findMemoryType(vk::PhysicalDevice const& physical_device, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties mem_properties = physical_device.getMemoryProperties();
	std::optional<uint32_t> result{};
	for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i)
			&&
			//All required properties must be present.
			(mem_properties.memoryTypes[i].propertyFlags & properties) == properties
			) {
			result = i;
			break;
		}
	}
	if (!result)
		throw std::runtime_error("failed to find suitable memory type!");
	return *result;
}

vk::CommandBuffer& BeginSingleTimeCBufferCmd(vk::CommandBuffer& cmd_buffer,vk::CommandBufferInheritanceInfo* info=nullptr)
{
	vk::DispatchLoaderDefault dispatcher{};
	cmd_buffer.reset(vk::CommandBufferResetFlags{}, dispatcher);
	//Setup copy command buffer/pool
	vk::CommandBufferBeginInfo beginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit,info };
	//Add the commands
	cmd_buffer.begin(beginInfo, dispatcher);

	return cmd_buffer;
}

void EndSingleTimeCbufferCmd(vk::CommandBuffer& cmd_buffer, vk::Queue& queue)
{
	vk::DispatchLoaderDefault dispatcher{};
	cmd_buffer.end(dispatcher);

	//Submit commands to queue
	vk::SubmitInfo submitInfo
	{
		 0
		,nullptr
		,nullptr
		,1
		,&cmd_buffer
	};
	queue.submit(submitInfo, vk::Fence{}, dispatcher);
	//Not very efficient, would be better to use fences instead.
	queue.waitIdle(dispatcher);
}

void CopyBuffer(vk::CommandBuffer& cmd_buffer, vk::Queue& queue, vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	BeginSingleTimeCBufferCmd(cmd_buffer);

	vk::DispatchLoaderDefault dispatcher{};	
	vk::BufferCopy copyRegion
	{
		0,0,size
	};
	cmd_buffer.copyBuffer(srcBuffer, dstBuffer, copyRegion, dispatcher);
	
	EndSingleTimeCbufferCmd(cmd_buffer,queue);
}

void CopyBufferToImage(vk::CommandBuffer& cmd_buffer, vk::Queue& queue, vk::Buffer& buffer, VknTexture& img)
{
	BeginSingleTimeCBufferCmd(cmd_buffer);

	vk::DispatchLoaderDefault dispatcher{};

	vk::BufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		static_cast<uint32_t>(img.size.x),
		static_cast<uint32_t>(img.size.y),
		1
	};

	cmd_buffer.copyBufferToImage(buffer, *img.vknData, vk::ImageLayout::eTransferDstOptimal, 1, &region, dispatcher);

	EndSingleTimeCbufferCmd(cmd_buffer, queue);
}

void TransitionImageLayout(vk::CommandBuffer& cmd_buffer, vk::Queue& queue, vk::Image& img, vk::Format format, vk::ImageLayout oLayout, vk::ImageLayout nLayout,vk::CommandBufferInheritanceInfo* info)
{
	BeginSingleTimeCBufferCmd(cmd_buffer,info);
	vk::DispatchLoaderDefault dispatcher{};

	//Creating image memory barrier to start performing layout transition for image
	vk::ImageMemoryBarrier vBarrier = {};
	vBarrier.oldLayout = oLayout;
	vBarrier.newLayout = nLayout;
	vBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vBarrier.image = img;
	vBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	vBarrier.subresourceRange.baseMipLevel = 0;
	vBarrier.subresourceRange.levelCount = 1;
	vBarrier.subresourceRange.baseArrayLayer = 0;
	vBarrier.subresourceRange.layerCount = 1;

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oLayout == vk::ImageLayout::eUndefined && nLayout == vk::ImageLayout::eTransferDstOptimal) {
		vBarrier.srcAccessMask = vk::AccessFlags::Flags();
		vBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oLayout == vk::ImageLayout::eTransferDstOptimal && nLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		vBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		vBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else {
		vBarrier.srcAccessMask = vk::AccessFlags::Flags();
		vBarrier.dstAccessMask = vk::AccessFlags::Flags();
		sourceStage = vk::PipelineStageFlagBits::eAllCommands;
		destinationStage = vk::PipelineStageFlagBits::eAllCommands;
		//throw std::invalid_argument("unsupported layout transition!");
	}

	//Pipeline barrier is impt so we need to go in-depth with this
	cmd_buffer.pipelineBarrier(
		sourceStage,
		destinationStage,
		vk::DependencyFlags(),
		0,nullptr,
		0,nullptr,
		1,&vBarrier,
		dispatcher
	);

	EndSingleTimeCbufferCmd(cmd_buffer, queue);
}

}

size_t Track(size_t s)
{
	static size_t allocated = 0;
	allocated += s;
	return s;
}

