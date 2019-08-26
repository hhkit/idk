#include "pch.h"
#include <vkn/BufferHelpers.h>
#include "VulkanDebugRenderer.h"
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


void CopyBuffer(vk::CommandBuffer& cmd_buffer, vk::Queue& queue, vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	vk::DispatchLoaderDefault dispatcher{};
	cmd_buffer.reset(vk::CommandBufferResetFlags{}, dispatcher);
	//Setup copy command buffer/pool
	vk::CommandBufferBeginInfo beginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
	//Add the commands
	cmd_buffer.begin(beginInfo, dispatcher);
	vk::BufferCopy copyRegion
	{
		0,0,size
	};
	cmd_buffer.copyBuffer(srcBuffer, dstBuffer, copyRegion, dispatcher);
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
}