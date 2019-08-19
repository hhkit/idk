#include "pch.h"
#include "VectorBuffer.h"
#include <VulkanHelpers.h>

namespace idk::vgfx
{
	VectorBuffer::VectorBuffer(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes)
	{
		Resize(pdevice, device, num_bytes);
	}

	size_t grow_size(size_t sz)
	{
		size_t new_size = 1;
		while (new_size < sz)
		{
			new_size <<= 1;
		}
		return new_size;
	}

	void VectorBuffer::Resize(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes, bool force_downsize)
	{
		auto dispatcher = vk::DispatchLoaderDefault{};
		if (num_bytes >= capacity || force_downsize)
		{
			capacity = grow_size(num_bytes);
			//if (buffer)
			//{
			//	
			//	auto tmp_mem = vhlp::AllocateBuffer(pdevice, device, *buffer, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, dispatcher);
			//	vhlp::BindBufferMemory(device, *buffer, *tmp_mem, 0, dispatcher);
			//	memory = std::move(tmp_mem);
			//}
			//else
			//{
				auto [buf, mem] = vhlp::CreateAllocBindBuffer(
					pdevice, device, capacity,
					vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
					vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible,
					dispatcher
				);
				memory = std::move(mem);
				buffer = std::move(buf);
			//}
		}
	}
	void VectorBuffer::Update(vk::DeviceSize offset, vk::DeviceSize len, vk::CommandBuffer& cmd_buffer, unsigned char const* data)
	{
		cmd_buffer.updateBuffer(*buffer, offset, vhlp::make_array_proxy(static_cast<uint32_t>(len), data), vk::DispatchLoaderDefault{});
	}

}