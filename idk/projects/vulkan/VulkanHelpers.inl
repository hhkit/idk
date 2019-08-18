#pragma once
#include <vulkan/vulkan.hpp>

template<typename Dispatcher>
vk::UniqueBuffer CreateBuffer(vk::Device device, vk::DeviceSize size, vk::BufferUsageFlags usage, Dispatcher const& dispatcher)
{

	vk::BufferCreateInfo bufferInfo
	{
		 vk::BufferCreateFlags{}
		,size
		,usage
		,vk::SharingMode::eExclusive             //Exclusive to the graphics queue family
	};
	return device.createBufferUnique(bufferInfo, nullptr, dispatcher);
}

template<typename T, typename Dispatcher>
vk::UniqueBuffer CreateVertexBuffer(vk::Device& device, T* const begin, T* const end, const Dispatcher& dispatcher)
{
	return CreateBuffer(device, buffer_size(begin, end), vk::BufferUsageFlagBits::eVertexBuffer, dispatcher);
}



template<typename T, typename Dispatcher>
vk::UniqueBuffer CreateVertexBuffer(vk::Device& device, std::vector<T> const& vertices, const Dispatcher& dispatcher)
{
	return CreateVertexBuffer(device, vertices.data(), vertices.data() + vertices.size(), dispatcher);
}
template<typename Dispatcher>
vk::UniqueDeviceMemory AllocateBuffer(
	vk::PhysicalDevice& pdevice, vk::Device& device, vk::Buffer const& buffer, vk::MemoryPropertyFlags memory_flags, Dispatcher const& dispatcher)
{
	vk::MemoryRequirements req = device.getBufferMemoryRequirements(buffer, dispatcher);

	vk::MemoryAllocateInfo allocInfo
	{
		 req.size
		,findMemoryType(pdevice,req.memoryTypeBits, memory_flags)
	};
	return device.allocateMemoryUnique(allocInfo, nullptr, dispatcher);
}
template<typename Dispatcher>
void BindBufferMemory(vk::Device& device, vk::Buffer& buffer, vk::DeviceMemory& memory, uint32_t offset, Dispatcher const& dispatcher)
{
	device.bindBufferMemory(buffer, memory, offset, dispatcher);
}
template<typename Dispatcher>
std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory> CreateAllocBindBuffer(
	vk::PhysicalDevice& pdevice, vk::Device& device,
	vk::DeviceSize buffer_size,
	vk::BufferUsageFlags buffer_usage,
	vk::MemoryPropertyFlags memory_flags,
	const Dispatcher& dispatcher
)
{
	auto buffer = CreateBuffer(device, buffer_size, buffer_usage, dispatcher);
	auto memory = AllocateBuffer(pdevice, device, *buffer, memory_flags, dispatcher);
	BindBufferMemory(device, *buffer, *memory, 0, dispatcher);
	return std::make_pair(std::move(buffer), std::move(memory));
}
template<typename T, typename Dispatcher>
std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory> CreateAllocBindVertexBuffer(
	vk::PhysicalDevice& pdevice, vk::Device& device, T const* vertices, T const* vertices_end, const Dispatcher& dispatcher
)
{

	return CreateAllocBindBuffer(pdevice, device, buffer_size(vertices, vertices_end), vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, dispatcher);
}
template<typename T, typename Dispatcher>
std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory> CreateAllocBindVertexBuffer(
	vk::PhysicalDevice& pdevice, vk::Device& device, std::vector<T> const& vertices, const Dispatcher& dispatcher
)
{
	return CreateAllocBindVertexBuffer(pdevice, device, vertices.data(), vertices.data() + vertices.size(), dispatcher);
}

template<typename T, typename Dispatcher>
void MapMemory(vk::Device& device, vk::DeviceMemory& memory, vk::DeviceSize dest_offset, T* src_start, vk::DeviceSize trf_size, Dispatcher const& dispatcher)
{
	vk::MappedMemoryRange mmr
	{
		 memory
		,dest_offset
		,trf_size
	};
	auto handle = device.mapMemory(memory, mmr.offset, mmr.size, vk::MemoryMapFlags{}, dispatcher);
	memcpy_s(handle, mmr.size, src_start, mmr.size);
	std::vector<decltype(mmr)> memory_ranges
	{
		mmr
	};
	//Not necessary rn since we set the HostCoherent bit 
	//This command only guarantees that the memory(on gpu) will be updated by vkQueueSubmit
	device.flushMappedMemoryRanges(memory_ranges, dispatcher);
	device.unmapMemory(memory);
}
