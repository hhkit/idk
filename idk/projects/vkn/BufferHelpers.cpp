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
//A version that allocs for you
vk::UniqueCommandBuffer BeginSingleTimeCBufferCmd(vk::Device device,vk::CommandPool pool, vk::CommandBufferInheritanceInfo* info)
{
	vk::CommandBufferAllocateInfo alloc_info
	{
		pool,vk::CommandBufferLevel::ePrimary,1
	};
	vk::DispatchLoaderDefault dispatcher{};
	auto cmd_buffers = device.allocateCommandBuffersUnique(alloc_info,dispatcher);
	auto cmd_buffer = std::move(cmd_buffers[0]);
	cmd_buffer->reset(vk::CommandBufferResetFlags{}, dispatcher);
	//Setup copy command buffer/pool
	vk::CommandBufferBeginInfo beginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit,info };
	//Add the commands
	cmd_buffer->begin(beginInfo, dispatcher);

	return std::move(cmd_buffer);
}

void EndSingleTimeCbufferCmd(vk::CommandBuffer cmd_buffer, vk::Queue queue,
	bool wait_for_idle,
	std::optional<vk::Fence> fence,
	std::optional<vk::Semaphore> wait,
	std::optional<vk::Semaphore> signal
)
{
	vk::Fence f{};
	if (fence)
		f = *fence;
	vk::DispatchLoaderDefault dispatcher{};
	cmd_buffer.end(dispatcher);
	try
	{

	//Submit commands to queue
	vk::SubmitInfo submitInfo
	{
		 (wait) ? 1U : 0U
		,(wait) ? &*wait: nullptr
		,nullptr
		,1
		,&cmd_buffer
		,(signal)?1U:0U
		,(signal) ? &*signal : nullptr
	};
	queue.submit(submitInfo, f, dispatcher);

	}
	catch (std::exception& e )
	{
		LOG_TO(LogPool::GFX, "Exception while ending single time cmd buffer: %s", e.what());
		DebugBreak();
	}
	catch (vk::Error & e)
	{
		LOG_TO(LogPool::GFX, "VkError while ending single time cmd buffer: %s", e.what());
		DebugBreak();

	}
	//Not very efficient, would be better to use fences instead.
	if(wait_for_idle)
		queue.waitIdle(dispatcher);
}
vk::CommandBuffer BeginSingleTimeCBufferCmd(vk::CommandBuffer cmd_buffer,vk::CommandBufferInheritanceInfo* info=nullptr)
{
	vk::DispatchLoaderDefault dispatcher{};
	cmd_buffer.reset(vk::CommandBufferResetFlags{}, dispatcher);
	//Setup copy command buffer/pool
	vk::CommandBufferBeginInfo beginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit,info };
	//Add the commands
	cmd_buffer.begin(beginInfo, dispatcher);

	return cmd_buffer;
}

void EndSingleTimeCbufferCmd(vk::CommandBuffer cmd_buffer, vk::Queue queue, std::optional<vk::Semaphore> wait = {}, std::optional<vk::PipelineStageFlags> stage = {}, std::optional<vk::Semaphore> signal = {}, std::optional<vk::Fence> fence = {})
{
	vk::DispatchLoaderDefault dispatcher{};
	cmd_buffer.end(dispatcher);

	//Submit commands to queue
	vk::SubmitInfo submitInfo
	{
		 (wait) ? 1u : 0u
		,(wait) ? &*wait : nullptr
		,(stage)?&*stage:nullptr
		,1
		,&cmd_buffer
		,(signal) ? 1u : 0u
		,(signal) ? &*signal : nullptr
	};
	vk::Fence f = {};
	if (fence)
		f = *fence;
	queue.submit(submitInfo, f, dispatcher);
	if(!signal)
	//Not very efficient, would be better to use fences instead.
		queue.waitIdle(dispatcher);
}

void CopyBuffer(vk::CommandBuffer cmd_buffer, vk::Queue queue, vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
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

void CopyBufferToImage(vk::CommandBuffer cmd_buffer, vk::Queue queue, vk::Buffer& buffer, VknTexture& img)
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

	cmd_buffer.copyBufferToImage(buffer, img.Image(), vk::ImageLayout::eTransferDstOptimal, 1, &region, dispatcher);

	EndSingleTimeCbufferCmd(cmd_buffer, queue);
}

// 


void TransitionImageLayout(vk::CommandBuffer cmd_buffer, vk::Queue queue, vk::Image img, vk::Format , vk::ImageLayout oLayout, vk::ImageLayout nLayout, TransitionOptions options)
{
	std::optional<BeginInfo> begin = options.begin;
	std::optional<SubmissionInfo> queue_sub_config = options.queue_sub_config;

	SubmissionInfo sub_info{};
	if (queue_sub_config)
		sub_info = *queue_sub_config;
	if (begin)
		BeginSingleTimeCBufferCmd(cmd_buffer, begin->info);
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
	sourceStage = vk::PipelineStageFlagBits::eAllCommands;
	destinationStage = vk::PipelineStageFlagBits::eAllCommands;
	vBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eTransferWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
	vBarrier.dstAccessMask = vk::AccessFlags::Flags();
	switch (oLayout)
	{
	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		sourceStage = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
		vBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
		vBarrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		break;
	case vk::ImageLayout::eTransferDstOptimal:
	{
		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		vBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
	}
	break;
	case vk::ImageLayout::eTransferSrcOptimal:
	{
		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		vBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
	}
	break;
	}
	switch (nLayout)
	{
	case vk::ImageLayout::eShaderReadOnlyOptimal:
		destinationStage = vk::PipelineStageFlagBits::eAllGraphics;
		vBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentRead;
		break;
	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
		vBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
		vBarrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		break;
	case vk::ImageLayout::eTransferDstOptimal:
	{
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
		vBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
	}
	break;
	case vk::ImageLayout::eTransferSrcOptimal:
	{
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
		vBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
	}
	break;
	}
	if ((oLayout == vk::ImageLayout::eUndefined || oLayout == vk::ImageLayout::ePreinitialized) && nLayout == vk::ImageLayout::eTransferDstOptimal) {
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
		//vBarrier.srcAccessMask = vk::AccessFlags::Flags();
		//vBarrier.dstAccessMask = vk::AccessFlags::Flags();
		//throw std::invalid_argument("unsupported layout transition!");
	}

	if (options.range)
		vBarrier.subresourceRange = *options.range;
	//Pipeline barrier is impt so we need to go in-depth with this
	cmd_buffer.pipelineBarrier(
		sourceStage,
		destinationStage,
		vk::DependencyFlags(),
		0, nullptr,
		0, nullptr,
		1, &vBarrier,
		dispatcher
	);

	if (begin)
		EndSingleTimeCbufferCmd(cmd_buffer, queue, sub_info.wait_for, sub_info.stage, sub_info.signal_after,sub_info.signal_fence);
}

}

size_t Track(size_t s)
{
	static size_t allocated = 0;
	allocated += s;
	return s;
}

