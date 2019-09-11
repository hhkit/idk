#pragma once
#include <vulkan/vulkan.hpp>
#include <vkn/VknTexture.h>
#include <vkn/MemoryAllocator.h>


namespace idk::vkn::hlp
{
	uint32_t findMemoryType(vk::PhysicalDevice const& physical_device, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
	vk::UniqueCommandBuffer BeginSingleTimeCBufferCmd(vk::Device device, vk::CommandPool pool, vk::CommandBufferInheritanceInfo* info = nullptr);


	void EndSingleTimeCbufferCmd(vk::CommandBuffer cmd_buffer, vk::Queue queue,
		bool wait_for_idle,
		std::optional<vk::Fence> fence = {},
		std::optional<vk::Semaphore> wait = {},
		std::optional<vk::Semaphore> signal = {}
	);

	template<typename Dispatcher>
	vk::UniqueBuffer CreateBuffer(vk::Device device, vk::DeviceSize size, vk::BufferUsageFlags usage, Dispatcher const& dispatcher);

	template<typename T, typename Dispatcher>
	vk::UniqueBuffer CreateVertexBuffer(vk::Device& device, T* const begin, T* const end, const Dispatcher& dispatcher);

	template<typename T, typename Dispatcher>
	vk::UniqueBuffer CreateVertexBuffer(vk::Device& device, std::vector<T> const& vertices, const Dispatcher& dispatcher);

	template<typename Dispatcher>
	vk::UniqueDeviceMemory AllocateBuffer(
		vk::PhysicalDevice& pdevice, vk::Device& device, vk::Buffer const& buffer, vk::MemoryPropertyFlags memory_flags, Dispatcher const& dispatcher);

	template<typename Dispatcher>
	void BindBufferMemory(vk::Device device, vk::Buffer buffer, vk::DeviceMemory memory, uint32_t offset, Dispatcher const& dispatcher);

	template<typename Dispatcher>
	std::pair<vk::UniqueBuffer, UniqueAlloc> CreateAllocBindBuffer(
		vk::PhysicalDevice& pdevice, vk::Device& device,
		vk::DeviceSize buffer_size,
		vk::BufferUsageFlags buffer_usage,
		vk::MemoryPropertyFlags memory_flags,
		MemoryAllocator& allocator,
		const Dispatcher& dispatcher
	);

	template<typename Dispatcher>
	std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory> CreateAllocBindBuffer(
		vk::PhysicalDevice& pdevice, vk::Device& device,
		vk::DeviceSize buffer_size,
		vk::BufferUsageFlags buffer_usage,
		vk::MemoryPropertyFlags memory_flags,
		const Dispatcher& dispatcher
	);

	template<typename T, typename Dispatcher>
	std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory> CreateAllocBindVertexBuffer(
		vk::PhysicalDevice& pdevice, vk::Device& device, T const* vertices, T const* vertices_end, const Dispatcher& dispatcher
	);

	template<typename T, typename Dispatcher>
	std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory> CreateAllocBindVertexBuffer(
		vk::PhysicalDevice& pdevice, vk::Device& device, std::vector<T> const& vertices, const Dispatcher& dispatcher
	);

	template<typename T, typename Dispatcher>
	void MapMemory(vk::Device& device, vk::DeviceMemory& memory, vk::DeviceSize dest_offset, T* src_start, vk::DeviceSize trf_size, Dispatcher const& dispatcher);

	void CopyBuffer(vk::CommandBuffer& cmd_buffer, vk::Queue& queue, vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

	void CopyBufferToImage(vk::CommandBuffer& cmd_buffer, vk::Queue& queue, vk::Buffer& buffer, VknTexture& img);

	void TransitionImageLayout(vk::CommandBuffer& cmd_buffer, vk::Queue& queue, vk::Image& img, vk::Format format, vk::ImageLayout oLayout, vk::ImageLayout nLayout, vk::CommandBufferInheritanceInfo* info = nullptr);

	template<typename T>
	vk::ArrayProxy<const T> make_array_proxy(uint32_t sz, T* arr);

	template<typename RT, typename T>
	RT buffer_size(T const& vertices);

	template<typename RT, typename T >
	RT buffer_size(T* begin, T* end);

	template<typename T, typename = void>
	struct ArrCount
	{
		static uint32_t count(const T& ) { return 1; }
	};
	template<typename T>
	struct ArrCount<T, decltype((void)std::size(std::declval<T>()))>
	{
		static uint32_t count(const T& t) { return static_cast<uint32_t>(std::size(t)); }
	};
	template<typename T>
	uint32_t arr_count(T&& arr) { return ArrCount<T>::count(arr); }
}

#include "BufferHelpers.inl"