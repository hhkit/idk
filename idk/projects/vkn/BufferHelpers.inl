#pragma once
#include <vulkan/vulkan.hpp>
#include "BufferHelpers.h"
size_t Track(size_t s);

namespace idk::vkn::hlp
{
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
	vk::UniqueBuffer CreateVertexBuffer(vk::Device device, T* const begin, T* const end, const Dispatcher& dispatcher)
	{
		return CreateBuffer(device, buffer_size(begin, end), vk::BufferUsageFlagBits::eVertexBuffer, dispatcher);
	}



	template<typename T, typename Dispatcher>
	vk::UniqueBuffer CreateVertexBuffer(vk::Device device, std::vector<T> const& vertices, const Dispatcher& dispatcher)
	{
		return CreateVertexBuffer(device, vertices.data(), vertices.data() + vertices.size(), dispatcher);
	}
// #pragma optimize("",off)
	template<typename Dispatcher>
	vk::UniqueDeviceMemory AllocateBuffer(
		vk::PhysicalDevice pdevice, vk::Device device, vk::Buffer const& buffer, vk::MemoryPropertyFlags memory_flags, Dispatcher const& dispatcher)
	{
		vk::MemoryRequirements req = device.getBufferMemoryRequirements(buffer, dispatcher);

		vk::MemoryAllocateInfo allocInfo
		{
			 Track(req.size)
			,findMemoryType(pdevice,req.memoryTypeBits, memory_flags)
		};
		return device.allocateMemoryUnique(allocInfo, nullptr, dispatcher);
	}

	template<typename Dispatcher>
	void BindBufferMemory(vk::Device device, vk::Buffer buffer, vk::DeviceMemory memory, uint32_t offset, Dispatcher const& dispatcher)
	{
		device.bindBufferMemory(buffer, memory, offset, dispatcher);
	}
	template<typename Dispatcher>
	std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory> CreateAllocBindBuffer(
		vk::PhysicalDevice pdevice, vk::Device device,
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
	template<typename Dispatcher>
	std::pair<vk::UniqueBuffer, UniqueAlloc> CreateAllocBindBuffer(
		vk::PhysicalDevice pdevice, vk::Device device,
		vk::DeviceSize buffer_size,
		vk::BufferUsageFlags buffer_usage,
		vk::MemoryPropertyFlags memory_flags,
		MemoryAllocator& allocator,
		const Dispatcher& dispatcher
	)
	{
		auto buffer = CreateBuffer(device, buffer_size, buffer_usage, dispatcher);
		auto memory = allocator.Allocate(pdevice, device, *buffer, memory_flags);
		BindBufferMemory(device, *buffer, memory->Memory(), s_cast<uint32_t>(memory->Offset()), dispatcher);
		return std::make_pair(std::move(buffer), std::move(memory));
	}
	template<typename T, typename Dispatcher>
	std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory> CreateAllocBindVertexBuffer(
		vk::PhysicalDevice pdevice, vk::Device device, T const* vertices, T const* vertices_end, const Dispatcher& dispatcher
	)
	{

		return CreateAllocBindBuffer(pdevice, device, buffer_size(vertices, vertices_end), vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, dispatcher);
	}
	template<typename T, typename Dispatcher>
	std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory> CreateAllocBindVertexBuffer(
		vk::PhysicalDevice pdevice, vk::Device device, std::vector<T> const& vertices, const Dispatcher& dispatcher
	)
	{
		return CreateAllocBindVertexBuffer(pdevice, device, vertices.data(), vertices.data() + vertices.size(), dispatcher);
	}

	template<typename T, typename Dispatcher>
	void MapMemory(vk::Device device, vk::DeviceMemory memory, vk::DeviceSize dest_offset, T* src_start, vk::DeviceSize trf_size, Dispatcher const& dispatcher)
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
		//device.flushMappedMemoryRanges(memory_ranges, dispatcher);
		device.unmapMemory(memory);
	}

	template<typename T>
	vk::ArrayProxy<const T> make_array_proxy(uint32_t sz, T* arr)
	{
		return vk::ArrayProxy<const T>{sz, arr};
	}

	namespace detail
	{
		template<typename T, typename = decltype(std::data(std::declval<T>()))>
		std::true_type has_data(const T& );
		template<typename T,typename ...Args>
		std::false_type has_data(T,Args...);
		template<typename T>
		static constexpr bool has_data_v = std::is_same_v<decltype(has_data(std::declval<T>())), std::true_type>;
	}
// #pragma optimize("",off)
	template<typename RT = size_t, typename T>
	RT buffer_size(T const& vertices)
	{
		if constexpr (detail::has_data_v<T>)
		{
			return static_cast<RT>(sizeof(*std::data(vertices)) * hlp::arr_count(vertices));
		}
		else
		{
			return static_cast<RT>(sizeof(vertices));
		}
	}

	template<typename RT = size_t, typename T = int>
	RT buffer_size(T * begin, T * end)
	{
		return static_cast<RT>(sizeof(T) * (end - begin));
	}

	template<typename T>
	auto buffer_data(T& obj)
	{
		if constexpr (detail::has_data_v<T>)
		{
			return std::data(obj);
		}
		else
		{
			return &obj;
		}
	}
#pragma optimize("",on)
}
